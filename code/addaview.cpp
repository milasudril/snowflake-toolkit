//@	{
//@	 "targets":[{"name":"addaview","type":"application"
//@ ,"dependencies":[{"ref":"GL","rel":"external"},{"ref":"GLEW","rel":"external"},{"ref":"glfw","rel":"external"}]}]
//@	}


#include "randomgenerator.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

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


static const char* shader_vertex_src="#version 330\n"
	"layout(location = 0) in vec3 vertex_pos_modelspace;\n"
	"uniform mat4 MVP;\n"
	"void main()\n"
	"	{\n"
	"	gl_Position=MVP*vec4(vertex_pos_modelspace,1);\n"
	"	}\n";

static const char* shader_fragment_src="#version 330\n"
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
	GlShader fragment_shader(GL_FRAGMENT_SHADER);
	auto ret=glCreateProgram();
	glAttachShader(ret,vertex_shader.idGet());
	glAttachShader(ret,fragment_shader.idGet());
	}



class PointCloud
	{
	public:
		PointCloud(const std::vector<glm::vec3>& points);
		void render() const noexcept;
	private:
		GLuint vertex_array;
		GLuint vbo;
		
		size_t n;
	};

PointCloud::PointCloud(const std::vector<glm::vec3>& points):n(points.size())
	{
	glGenVertexArrays(1,&vertex_array);
	glBindVertexArray(vertex_array);

//	TODO: Compile shaders...

	glEnable(GL_DEPTH_TEST);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,n*sizeof(glm::vec3),points.data(),GL_STATIC_DRAW);
	}

void PointCloud::render() const noexcept
	{
	glPointSize(1);
	glDrawArrays(GL_POINTS,0,n);
	}


static void render(GLFWwindow* handle)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	auto pc=reinterpret_cast<const PointCloud*>(glfwGetWindowUserPointer(handle));
	pc->render();
	glfwSwapBuffers(handle);
	}

static std::vector<glm::vec3> pointsLoad()
	{
	SnowflakeModel::RandomGenerator rng;
	std::uniform_real_distribution<float> U(-1,-1);
	std::vector<glm::vec3> ret;
	for(size_t k=0;k<1000;++k)
		{ret.push_back({U(rng),U(rng),U(rng)});}
	return std::move(ret);
	}

int main()
	{
	try
		{
		GLFWGuard glfw;
		auto window=windowCreate(640,480,"addaview");
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


		glfwSetWindowCloseCallback(window.get(),close);
		glfwSetWindowRefreshCallback(window.get(),render);

		PointCloud cloud(pointsLoad());
		glfwSetWindowUserPointer(window.get(),&cloud);

		while (!glfwWindowShouldClose(window.get()))
			{
			glfwWaitEvents();
			}

		}
	catch(const char* message)
		{
		fprintf(stderr,"Error: %s\n",message);return -1;
		}


	return 0;
	}
