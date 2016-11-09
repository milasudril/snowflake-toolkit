//@	{
//@	 "targets":
//@		[{
//@		 "name":"mesh_shadow","type":"application"
//@		,"description":"Mesh shadow renderer"		
//@ 	,"dependencies":
//@			[
//@			 {"ref":"png","rel":"external"}
//@			,{"ref":"GL","rel":"external"}
//@			,{"ref":"GLEW","rel":"external"}
//@			,{"ref":"glfw","rel":"external"}
//@			]
//@		}]
//@	}

#include "solid.h"
#include "file_in.h"
#include "file_out.h"
#include "ice_particle.h"
#include "solid_loader.h"
#include "filename.h"
#include "config_parser.h"
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
#include <png.h>

namespace Alice
	{
	template<>
	struct MakeType<Stringkey("angle")>:public MakeType<Stringkey("float")>
		{
		static constexpr const char* descriptionShortGet() noexcept
			{return "A value within [0, 1]";}
		static constexpr const char* descriptionLongGet() noexcept
			{return "Angles are normalized. This means that 1 corresponds to a full turn, or 2 pi";}
		};

	template<>
	struct MakeType<Stringkey("shadow map file")>:public MakeType<Stringkey("filename")>
		{
		static constexpr const char* descriptionShortGet() noexcept
			{return "filename";}
		static constexpr const char* descriptionLongGet() noexcept
			{
			return "A filename that refers to a file that contains a shadow map. "
				"A shadow map is encoded as a PNG. The image resolution is the "
				"number of pixels per length unit, multiplied by 1024.";
			}
		};
	}

ALICE_OPTION_DESCRIPTOR(OptionDescriptor
	,{"Help","help","Print option summary to stdout, or, if a filename is given, to that file","filename",Alice::Option::Multiplicity::ZERO_OR_ONE}
	,{"Input","prototype","Mesh to render. If this option is not given, the data is read from stdin.","filename",Alice::Option::Multiplicity::ONE}
	,{"Input","alpha","First Euler angle","angle",Alice::Option::Multiplicity::ONE}
	,{"Input","beta","Second Euler angle","angle",Alice::Option::Multiplicity::ONE}
	,{"Input","gamma","Third Euler angle","angle",Alice::Option::Multiplicity::ONE}
	,{"Output","image","Destination file. If this option is not given, the image is written to stdout","shadow map file",Alice::Option::Multiplicity::ONE}
	,);


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
	glfwWindowHint(GLFW_VISIBLE,0);
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
	return std::unique_ptr<GLFWwindow,GLFWDeleter>(glfwCreateWindow(width,height,title,NULL,NULL),GLFWDeleter());
	}

static void close(GLFWwindow* handle)
	{
	glfwSetWindowShouldClose(handle,1);
	}


static const char* g_vertex_shader_src="#version 330\n"
	"layout(location = 0) in vec3 vertex_pos_modelspace;\n"
	"uniform mat4 MVP;\n"
	"void main()\n"
	"	{\n"
	"	gl_Position=MVP*vec4(vertex_pos_modelspace,1);\n"
	"	}\n";

static const char* g_fragment_shader_src="#version 330\n"
	"uniform vec3 fragment_color;\n"
	"out vec3 color;\n"
	"void main()\n"
	"	{\n"
	"	color=fragment_color;\n"
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



class ShadowMask
	{
	public:
		ShadowMask(const ShadowMask&)=delete;
		ShadowMask& operator=(const ShadowMask&)=delete;

		ShadowMask(ShadowMask&& b)=default;

		ShadowMask(const SnowflakeModel::Solid& s);

		void render(float alpha,float beta,float gamma
			,const glm::mat4& projection) const noexcept;

		float scaleGet() const noexcept
			{return m_s;}

	private:
		GLuint vertex_array;
		GLuint vbo;
		GLuint element_buffer;
		GLuint shader;
		GLuint mvp_location;
		GLuint fragment_color_location;
		glm::mat4 scalepos;
		float m_s;
		size_t n;
		size_t n_faces;
	};

std::vector<SnowflakeModel::Vector> verticesGet(const SnowflakeModel::Solid& solid)
	{
	std::vector<SnowflakeModel::Vector> ret;
	auto ptr=solid.subvolumesBegin();
	auto ptr_end=solid.subvolumesEnd();
	while(ptr!=ptr_end)
		{
		auto v=ptr->verticesBegin();
		auto v_end=ptr->verticesEnd();
		while(v!=v_end)
			{
			ret.push_back(SnowflakeModel::Vector(*v));
			++v;
			}
		++ptr;
		}
	return std::move(ret);
	}

std::vector<uint32_t> facesGet(const SnowflakeModel::Solid& solid)
	{
	std::vector<uint32_t> ret;
	auto ptr=solid.subvolumesBegin();
	auto ptr_end=solid.subvolumesEnd();
	uint32_t vi_start=0;
	while(ptr!=ptr_end)
		{
		auto v=ptr->facesBegin();
		auto v_end=ptr->facesEnd();
		while(v!=v_end)
			{
			ret.push_back(vi_start+v->vertexGet(0));
			ret.push_back(vi_start+v->vertexGet(1));
			ret.push_back(vi_start+v->vertexGet(2));
			++v;
			}
		vi_start+=ptr->verticesCount();
		++ptr;
		}
	return std::move(ret);
	}

ShadowMask::ShadowMask(const SnowflakeModel::Solid& solid)
	{
	glGenVertexArrays(1,&vertex_array);
	glBindVertexArray(vertex_array);
	shader=createShaderProgram(g_vertex_shader_src,g_fragment_shader_src);
	glUseProgram(shader);
	mvp_location=glGetUniformLocation(shader,"MVP");
	fragment_color_location=glGetUniformLocation(shader,"fragment_color");

	glGenBuffers(1,&vbo);
	glGenBuffers(1,&element_buffer);

	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	auto verts=verticesGet(solid);
	n=verts.size();
	glPointSize(10);
	glBufferData(GL_ARRAY_BUFFER,n*sizeof(glm::vec3),verts.data(),GL_STATIC_DRAW);
	auto faces=facesGet(solid);
	n_faces=faces.size();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,n_faces*sizeof(uint32_t)
		,faces.data(),GL_STATIC_DRAW);

	auto bb=solid.boundingBoxGet();
	auto mid=0.5f*(bb.m_min + bb.m_max);
	auto s=0.5f*glm::length(bb.m_max - bb.m_min);
	scalepos=glm::scale(scalepos,SnowflakeModel::Vector(1.0f,1.0f,1.0f)/s);
	scalepos=glm::translate(scalepos,SnowflakeModel::Vector(-mid));
	m_s=0.5f/s;
	}

void ShadowMask::render(float alpha,float beta,float gamma
	,const glm::mat4& projection) const noexcept
	{
	glm::mat4 mvp;
	mvp=glm::rotate(mvp,alpha,glm::vec3(0,0,1));
	mvp=glm::rotate(mvp,beta,glm::vec3(0,1,0));
	mvp=glm::rotate(mvp,gamma,glm::vec3(0,0,1));
	mvp=mvp*scalepos;
	mvp=projection*mvp;

	glUniformMatrix4fv(mvp_location, 1, GL_FALSE, &mvp[0][0]);
	glUniform3f(fragment_color_location,1,1,1);	
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,element_buffer);
	glDrawElements(GL_TRIANGLES,n_faces,GL_UNSIGNED_INT,nullptr);
	glDisableVertexAttribArray(0);
	}

static void pixelsDump(GLFWwindow* window,float scale
	,SnowflakeModel::FileOut&& dump)
	{
	int width;
	int height;
	glfwGetWindowSize(window,&width,&height);

	std::vector<uint8_t> ret(width*height);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glReadPixels(0,0,width,height,GL_GREEN,GL_UNSIGNED_BYTE,ret.data());
	auto pngptr=png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	auto pnginfo=png_create_info_struct(pngptr);
	png_init_io(pngptr, dump.handleGet());
	png_set_IHDR(pngptr,pnginfo,width,height,8,PNG_COLOR_TYPE_GRAY,PNG_INTERLACE_NONE
		,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);
	png_set_pHYs(pngptr,pnginfo,1024.0f*width*scale + 0.5f,1024.0f*width*scale + 0.5f
		,PNG_RESOLUTION_UNKNOWN);
	png_set_sCAL(pngptr,pnginfo,1,1/(width*scale),1/(width*scale));
	png_write_info(pngptr,pnginfo);
	for(int k=height-1;k>=0;--k)
		{png_write_row(pngptr, ret.data() + width*k);}
	png_write_end(pngptr, pnginfo);
	png_destroy_write_struct(&pngptr,&pnginfo);
	}


struct ViewState
	{
	const ShadowMask& pc;
	float alpha;
	float beta;
	float gamma;
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
	auto projection=make_ortho(width,height,1.0f,10,20);
	glClear(GL_COLOR_BUFFER_BIT);
	vs->pc.render(vs->alpha,vs->beta,vs->gamma,projection);
	
	glfwSwapBuffers(handle);
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


static SnowflakeModel::Solid solidLoad(const std::string& prototype)
	{
	if(prototype.size()==0)
		{throw "No crystal prototype is given";}

	SnowflakeModel::Solid solid_in;
		{
		SnowflakeModel::FileIn file_in(prototype.c_str());
		SnowflakeModel::ConfigParser parser(file_in);
		SnowflakeModel::SolidLoader loader(solid_in);
		parser.commandsRead(loader);
		}

	return std::move(solid_in);
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
		auto window=windowCreate(1024,1024,"mesh shadow");
		glfwMakeContextCurrent(window.get());
		glewExperimental=1;
		fprintf(stderr,"Initializing GLEW %s\n",reinterpret_cast<const char*>(glewGetString(GLEW_VERSION)));
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

		auto mask=[&cmdline,&window]()
			{
			auto solid_in=solidLoad(cmdline.get<Alice::Stringkey("prototype")>().valueGet());
			glfwSetWindowTitle(window.get(),"Mesh shadow");
			SnowflakeModel::IceParticle particle_out;
			particle_out.solidSet(solid_in);
			return ShadowMask(particle_out.solidGet());
			}();

		ViewState vs
			{
			 mask
			,2.0f*std::acos(-1.0f)*cmdline.get<Alice::Stringkey("alpha")>().valueGet()
			,2.0f*std::acos(-1.0f)*cmdline.get<Alice::Stringkey("beta")>().valueGet()
			,2.0f*std::acos(-1.0f)*cmdline.get<Alice::Stringkey("gamma")>().valueGet()
			};
		glfwSetWindowUserPointer(window.get(),&vs);

		glfwSetWindowCloseCallback(window.get(),close);
		glfwSetWindowRefreshCallback(window.get(),render);
		glfwSetWindowPos(window.get(), 50, 50);
		glfwSetWindowSize(window.get(),1024,1024);

		glfwWaitEvents();
		render(window.get());
		render(window.get());
		pixelsDump(window.get()
			,mask.scaleGet()
			,SnowflakeModel::FileOut(cmdline.get<Alice::Stringkey("image")>()?
				 SnowflakeModel::FileOut(cmdline.get<Alice::Stringkey("image")>().valueGet().c_str())
				:SnowflakeModel::FileOut(stdout))
			);
		}
	catch(const Alice::ErrorMessage& message)
		{
		fprintf(stderr,"Command line error: %s\n",message.data);return -1;
		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);return -1;
		}


	return 0;
	}
