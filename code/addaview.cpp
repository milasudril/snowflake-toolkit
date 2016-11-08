//@	{
//@	 "targets":
//@		[{
//@		 "name":"addaview","type":"application"
//@ 	,"dependencies":
//@			[
//@			 {"ref":"GL","rel":"external"}
//@			,{"ref":"GLEW","rel":"external"}
//@			,{"ref":"glfw","rel":"external"}
//@			]
//@		,"description":"ADDA point cloud visualizer"
//@		}]
//@	}


#include "randomgenerator.h"
#include "file_in.h"
#include "file_out.h"
#include "filename.h"
#include "alice/commandline.hpp"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Input","file","adda file to render","filename",Alice::Option::Multiplicity::ONE});

[[noreturn]] static void glfwErrorRaise(int code,const char* message)
	{
	fprintf(stderr,"GLFW error: %s\n",message);
	abort();
	}

static void init()
	{
	fprintf(stderr,"Initializing GLFW version %s",glfwGetVersionString());
	glfwSetErrorCallback(glfwErrorRaise);
	glfwInit();
	glfwWindowHint(GLFW_SAMPLES,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER,GL_TRUE);
	}

static void deinit()
	{
	glfwTerminate();
	}

struct GLFWGuard
	{
	public:
		GLFWGuard(){init();}
		~GLFWGuard(){deinit();}
	};

struct GLFWDeleter
	{
	void operator()(GLFWwindow* win) noexcept
		{glfwDestroyWindow(win);}
	};

static std::unique_ptr<GLFWwindow,GLFWDeleter> windowCreate(unsigned int width,unsigned int height,const char* title)
	{
	return std::unique_ptr<GLFWwindow,GLFWDeleter>(glfwCreateWindow(width+1,height,title,NULL,NULL),GLFWDeleter());
	}

static void close(GLFWwindow* handle)
	{
	glfwSetWindowShouldClose(handle,1);
	}


static const char* g_vertex_shader_src="#version 330\n"
	"layout(location = 0) in vec3 vertex_pos_modelspace;\n"
	"uniform mat4 MVP;\n"
	"uniform mat4 scalepos;\n"
	"out vec3 pos_worldspace;\n"
	"void main()\n"
	"	{\n"
	"	pos_worldspace=vec3( scalepos*vec4(vertex_pos_modelspace,1) );\n"
	"	gl_Position=MVP*vec4(vertex_pos_modelspace,1);\n"
	"	}\n";

static const char* g_fragment_shader_src="#version 330\n"
	"uniform vec3 fragment_color;\n"
	"out vec3 color;\n"
	"in vec3 pos_worldspace;\n"
	"void main()\n"
	"	{\n"
	"	vec3 color_coord=0.5f*( vec3(1,1,1) + pos_worldspace );\n"
	"	color=fragment_color*color_coord;\n"
	"	}\n";

namespace
	{
	class GlShader
		{
		public:
			explicit GlShader(GLenum type)
				{
				id=glCreateShader(type);
				}

			GlShader& sourceSet(const char* source) noexcept
				{
				glShaderSource(id,1,&source,NULL);
				return *this;
				}

			void compile();

			~GlShader()
				{glDeleteShader(id);}

			GLuint idGet() const noexcept
				{return id;}

		private:
			GLuint id;
		};
	}
void GlShader::compile()
	{
	glCompileShader(id);
	GLint result;
	glGetShaderiv(id,GL_COMPILE_STATUS,&result);
	if(result==GL_FALSE)
		{
		glGetShaderiv(id,GL_INFO_LOG_LENGTH,&result);
		if(result>0)
			{
			char buffer[512];
			glGetShaderInfoLog(id,512,NULL,buffer);
			fprintf(stderr,"Could not compile shader: %s",buffer);
			throw "Shader compilation error";
			}
		}
	}

static GLuint createShaderProgram(const char* vertex_shader_src
	,const char* fragment_shader_src)
	{
	GlShader vertex_shader(GL_VERTEX_SHADER);
	vertex_shader.sourceSet(vertex_shader_src);
	vertex_shader.compile();
	GlShader fragment_shader(GL_FRAGMENT_SHADER);
	fragment_shader.sourceSet(fragment_shader_src);
	fragment_shader.compile();
	auto ret=glCreateProgram();
	glAttachShader(ret,vertex_shader.idGet());
	glAttachShader(ret,fragment_shader.idGet());
	glLinkProgram(ret);
	glDetachShader(ret, vertex_shader.idGet());
	glDetachShader(ret, fragment_shader.idGet());
	return ret;
	}



class PointCloud
	{
	public:
		PointCloud(const PointCloud&)=delete;
		PointCloud& operator=(const PointCloud&)=delete;

		PointCloud(PointCloud&& b)=default;

		PointCloud(const std::vector<glm::vec3>& points,const glm::vec3& mid
			,const glm::vec3& radius);

		void render(float distance,float azimuth,float zenith
			,const glm::mat4& projection) const noexcept;

		size_t nPointsGet() const noexcept
			{return n;}

	private:
		GLuint vertex_array;
		GLuint vbo;
		GLuint shader;
		GLuint mvp_location;
		GLuint fragment_color_location;
		GLuint scalepos_location;
		glm::mat4 scalepos;
		size_t n;
	};

PointCloud::PointCloud(const std::vector<glm::vec3>& points
	,const glm::vec3& mid
	,const glm::vec3& radius):n(points.size())
	{
	glGenVertexArrays(1,&vertex_array);
	glBindVertexArray(vertex_array);
	shader=createShaderProgram(g_vertex_shader_src,g_fragment_shader_src);
	glUseProgram(shader);
	mvp_location=glGetUniformLocation(shader,"MVP");
	fragment_color_location=glGetUniformLocation(shader,"fragment_color");
	scalepos_location=glGetUniformLocation(shader,"scalepos");

	glEnable(GL_DEPTH_TEST);
	glPointSize(2);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,n*sizeof(glm::vec3),points.data(),GL_STATIC_DRAW);


	auto s=std::max(radius.x,std::max(radius.y,radius.z));
	scalepos=glm::scale(scalepos,glm::vec3(1.0f,1.0f,1.0f)/s);
	scalepos=glm::translate(scalepos,-mid);
	glUniformMatrix4fv(scalepos_location, 1, GL_FALSE, &scalepos[0][0]);
	}

void PointCloud::render(float distance,float azimuth,float zenith,const glm::mat4& projection) const noexcept
	{
	glm::mat4 mvp;
	mvp=glm::translate(mvp,glm::vec3(0.0f,0.0f,-distance));
	mvp=glm::rotate(mvp,zenith,glm::vec3(1,0,0));
	mvp=glm::rotate(mvp,azimuth,glm::vec3(0,0,1));
	mvp=mvp*scalepos;
	mvp=projection*mvp;

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
	glUniform3f(fragment_color_location,1,1,1);	
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glDrawArrays(GL_POINTS,0,n);
	glDisableVertexAttribArray(0);
	}

struct ViewState
	{
	const PointCloud& pc;
	float distance;
	float view_thickness;
	float azimuth;
	float zenith;

	double x_0;
	double y_0;
	float thickness_min;
	bool orth;
	bool shift;
	bool ctrl;
	};

static glm::mat4 make_ortho(float width,float height,float size,float distance
	,float view_thickness)
	{
	auto r=width<height?size/width:size/height;

	width*=r;
	height*=r;


	auto left=-width;
	auto right=-left;
	auto bottom=-height;
	auto top=-bottom;

	return glm::ortho(left,right,bottom,top,-distance,-distance+view_thickness);
	}


static void render(GLFWwindow* handle)
	{
	int width;
	int height;
	glfwGetWindowSize(handle,&width,&height);
	auto vs=reinterpret_cast<const ViewState*>(glfwGetWindowUserPointer(handle));
	
	glViewport(0,0,width,height);
	auto projection=vs->orth?make_ortho(width,height,1.25f,vs->distance,vs->view_thickness):
		glm::perspective(glm::radians(45.0f), (float) width / (float)height, 0.1f, 100.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	vs->pc.render(vs->distance,vs->azimuth,vs->zenith,projection);
	
	glfwSwapBuffers(handle);
	}

static void mouseMove(GLFWwindow* handle,double x,double y)
	{
	int width;
	int height;
	glfwGetWindowSize(handle,&width,&height);

	auto vs=reinterpret_cast<ViewState*>(glfwGetWindowUserPointer(handle));
	if(glfwGetMouseButton(handle,GLFW_MOUSE_BUTTON_1)==GLFW_PRESS)
		{
		vs->zenith+=std::acos(-1.0f)*(y - vs->y_0)/600.0f;
		vs->azimuth+=std::acos(-1.0f)*(x - vs->x_0)/600.0f;
		}
	vs->y_0=y;
	vs->x_0=x;
	}

static void scroll(GLFWwindow* handle,double x,double y)
	{
	auto vs=reinterpret_cast<ViewState*>(glfwGetWindowUserPointer(handle));
	if(vs->orth)
		{
		if(vs->ctrl)
			{
			auto thickness_new=static_cast<float>(vs->view_thickness + ((!vs->shift)?y/64.0f:y/8.0f));
			vs->view_thickness=std::max(vs->thickness_min,thickness_new);
			}
		else
			{vs->distance-= (!vs->shift)?y/32.0f:y/8.0f;}
		}
	else
		{vs->distance-=y/8.0f;}
	}

static void keyAction(GLFWwindow* handle,int key,int scancode,int action,int mods)
	{
	auto vs=reinterpret_cast<ViewState*>(glfwGetWindowUserPointer(handle));
	if(action==GLFW_PRESS)
		{
		switch(key)
			{
			case 32:
				vs->distance=6.0f;
				vs->view_thickness=12.0f;
				vs->azimuth=0;
				vs->zenith=-std::acos(-1.0f)/2.0f;
				break;
			case 'O':
				vs->orth=!vs->orth;
				break;
			}
		if(scancode==50 || mods&GLFW_MOD_SHIFT)
			{vs->shift=1;}
		if(scancode==37 || mods&GLFW_MOD_CONTROL)
			{vs->ctrl=1;}
		}
	else
		{
		if(scancode==50)
			{vs->shift=0;}
		if(scancode==37)
			{vs->ctrl=0;}
		}
	}

static void helpPrint(const Alice::CommandLine<OptionDescriptor>& options
	,const std::vector<std::string>& dest)
	{
	if(dest.size()==0)
		{options.help(1,stdout);}
	else
		{
		SnowflakeModel::FileOut output(dest[0].c_str());
		options.help(1,output.handleGet());
		}
	}

static std::vector<glm::vec3> pointsLoad(const char* source)
	{
	auto f=source==nullptr?
		SnowflakeModel::FileIn(stdin):SnowflakeModel::FileIn(source);
	int ch_in=0;
	std::string buffer;
	int state=0;
	int index=0;
	glm::vec3 point;
	std::vector<glm::vec3> ret;
	while((ch_in=f.getc())!=EOF)
		{
		switch(state)
			{
			case 0:
				switch(ch_in)
					{
					case '#':
						state=1;
						if(index!=0)
							{throw "Uncompleted point";}
						break;
					case '\r':
						break;
					case'\n':
						point[index]=atof(buffer.c_str());
						ret.push_back(point);
						buffer.clear();
						index=0;
						break;
					case ' ':
						point[index]=atof(buffer.c_str());
						buffer.clear();
						++index;
						if(index==3)
							{throw "Too many values";}
						break;
					default:
						buffer+=ch_in;
					}
				break;
			case 1:
				switch(ch_in)
					{
					case '\n':
					case '\r':
						state=0;
						buffer.clear();
						break;
					default:
						break;
					}
				break;
			}
		}
	return std::move(ret);
	}

static std::vector<glm::vec3> pointsLoad()
	{
	SnowflakeModel::RandomGenerator rng;
	std::uniform_real_distribution<float> U(0,0.5f);
	std::vector<glm::vec3> ret;
	for(size_t k=0;k<10000;++k)
		{ret.push_back({U(rng),U(rng),U(rng)});}
	return ret;
	}

static std::pair<glm::vec3,glm::vec3> boundingBoxGet(const std::vector<glm::vec3>& points)
	{
	auto ptr=points.data();
	auto ptr_end=ptr + points.size();
	if(ptr==ptr_end)
		{return std::pair<glm::vec3,glm::vec3>{{-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f}};}
	++ptr;
	auto ret=std::pair<glm::vec3,glm::vec3>{*ptr,*ptr};
	while(ptr!=ptr_end)
		{
		ret.first=glm::min(*ptr,ret.first);
		ret.second=glm::max(*ptr,ret.second);
		++ptr;
		}

	return ret;
	}

int main(int argc,char** argv)
	{
	try
		{
		Alice::CommandLine<OptionDescriptor> cmdline(argc,argv);
			{
			auto& x=cmdline.get<Alice::Stringkey("help")>();
			if(x)
				{
				helpPrint(cmdline,x.valueGet());
				return 0;
				}
			}

		GLFWGuard glfw;
		auto window=windowCreate(640,480,"adda shape view");
		glfwMakeContextCurrent(window.get());
		glewExperimental=1;
		fprintf(stderr,"Initializing GLEW %s",reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
		auto glew_result=glewInit();
		if(glew_result != GLEW_OK)
			{throw "Faield to initialize GLEW";	}

		fprintf(stderr,"Got an OpenGL context with the "
			"following characteristics:\n\n"
			"  Vendor:       %s\n"
			"  Renderer:     %s\n"
			"  Version:      %s\n"
			"  GLSL version: %s\n"
			,reinterpret_cast<const char*>(glGetString(GL_VENDOR))
			,reinterpret_cast<const char*>(glGetString(GL_RENDERER))
			,reinterpret_cast<const char*>(glGetString(GL_VERSION))
			,reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

		auto cloud=[&cmdline,&window]()
			{
			auto& x=cmdline.get<Alice::Stringkey("file")>();
			auto points=pointsLoad(x?x.valueGet().c_str():nullptr);
		//	auto points=pointsLoad();
			fprintf(stderr,"Loaded %zu points\n",points.size());
			auto bb=boundingBoxGet(points);
			auto mid=0.5f*(bb.second + bb.first);
			auto radius=0.5f*(bb.second - bb.first);
			auto wintitle=std::string("adda shape view ") + (x?x.valueGet().c_str():"/dev/stdin") 
				+ " " + std::to_string(points.size()) + " points";
			glfwSetWindowTitle(window.get(),wintitle.c_str());
			return PointCloud(points,mid,radius);
			}();

		ViewState vs{cloud,6.0f,12.0f,0,-std::acos(-1.0f)/2.0f,0,0
			,static_cast<float>(2.0f*std::pow(cloud.nPointsGet(),-1.0/3.0))};
		glfwSetWindowUserPointer(window.get(),&vs);

		glfwSetWindowCloseCallback(window.get(),close);
		glfwSetWindowRefreshCallback(window.get(),render);
		glfwSetCursorPosCallback(window.get(),mouseMove);
		glfwSetScrollCallback(window.get(),scroll);
		glfwSetKeyCallback(window.get(),keyAction);


		fprintf(stderr,"\n\nNavigation\n"
			"==========\n"
			" * Drag to look from different angles [Left mouse + move]\n"
			" * Use scroll wheel to modify view distance\n"
			" * Space bar resets camera position\n"
			" * O toggles orthographic view\n"
			"\n"
			"Orthographic view also features the following modes\n"
			" * Shift icreases distance changes\n"
			" * Ctrl + Wheel changes the view thickness. This makes it possible "
			"to create a slice\n");

		while (!glfwWindowShouldClose(window.get()))
			{
			glfwWaitEvents();
			render(window.get());
			}

		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);return -1;
		}


	return 0;
	}
