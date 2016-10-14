//@	{
//@	 "targets":[{"name":"addaview","type":"application"
//@ ,"dependencies":[{"ref":"GL","rel":"external"},{"ref":"GLEW","rel":"external"},{"ref":"glfw","rel":"external"}]}]
//@	}

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

class PointCloud
	{
	public:
		PointCloud(const glm::vec3* points,size_t N);
		void render() const noexcept;
	private:
		GLuint vertex_array;
		GLuint vbo;
		size_t n;
	};

PointCloud::PointCloud(const glm::vec3* points,size_t N):n(N)
	{
	glGenVertexArrays(1,&vertex_array);
	glBindVertexArray(vertex_array);

//	TODO: Compile shaders...

	glEnable(GL_DEPTH_TEST);
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,N*sizeof(glm::vec3),points,GL_STATIC_DRAW);
	}

void PointCloud::render() const noexcept
	{
	glPointSize(2);
	glDrawArrays(GL_POINTS,0,n);
	}


static void render(GLFWwindow* handle)
	{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	auto pc=reinterpret_cast<const PointCloud*>(glfwGetWindowUserPointer(handle));
	pc->render();
	glfwSwapBuffers(handle);
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

		std::vector<glm::vec3> points;
		points.push_back({0.0f,0.0f,0.0f});
		PointCloud cloud(points.data(),points.size());
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
