#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <signal.h>
#include "boilerplate.h"

#define BUFFER_OFFSET(i) ((void*)(i))
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "helpers.h"

extern void show_backtrace(void);
extern void backtrace(void);

void handler(int sig) {
	backtrace();
	exit(1);
}

extern GLuint png_texture_load(const char* file_name, int* width, int* height);

std::string operator "" s (const char* p, size_t) {
	return std::string(p);
}
#define SHADER_DIR "./shaders"s
namespace ns {

template <typename T, typename U>
void foo(T t, U u) {
  backtrace(); // <-------- backtrace here!
}

}  // namespace ns

template <typename T>
struct Klass {
  T t;
  void bar() {
    ns::foo(t, true);
  }
};


void outputShaderLog(unsigned int shaderID) {
  std::vector<char> infoLog;
  GLint infoLen;
  glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
  infoLog.resize(infoLen);

  glGetShaderInfoLog(shaderID, infoLen, &infoLen, &infoLog[0]);

  std::cout << std::string(infoLog.begin(), infoLog.end()) << std::endl;
}
int main(int argc, char *argv[]) {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
	auto shaders = std::map<std::string, std::string>();
	auto d = list_files(SHADER_DIR);
	//printf("shd: %d\n", d.size());
	for (std::string s : d) {
		shaders[s] = (read_file(SHADER_DIR+"/"+s));
		printf("%s\n---\n%s\n---\n", s.c_str(), shaders[s].c_str());
	}
	glx::setup_x();
	int ww,hh;
	int texture = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	printf("Texture 1: %d\n", texture);
	//int texture2 = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	XEvent xev;
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	  
	}
	glViewport(0,0,800,600);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto projection = glm::ortho( 0.f, 800.f, 600.f, 0.0f, 0.0f, 100.f ); 
	glm::mat4 VP = glm::mat4();
	glm::mat4 view = glm::lookAt(
				glm::vec3(1,1,1), // Camera is at (0,0,5), in World Space
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,0,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	VP = projection;
	auto program = glCreateProgram();
	auto asdf = shaders["basic.glsl"].c_str();
	auto fdsa = shaders["frag.glsl"].c_str();
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &asdf, NULL);
		glCompileShader(vs);
		int result;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("vert fail\n");
			exit(1);
		}
		glAttachShader(program, vs);
		//glLinkProgram(program);
	GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fdsa, NULL);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("frag fail\n");
			outputShaderLog(fs);
			exit(1);
		}
		glAttachShader(program, fs);
		glLinkProgram(program);
	
	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	int x = 4;
	int y = 5;
	const float TILE_SIZE = 32.0f;
	float vertices[] ={
		x*TILE_SIZE,		y*TILE_SIZE,		0.0f,
		//1.0f, 1.0f,
		(x+1)*TILE_SIZE,	y*TILE_SIZE,		0.0f,
		//1.0f, 1.0f,
		(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	0.0f,
		//1.0f, 1.0f,
		x*TILE_SIZE,		(y+1)*TILE_SIZE,	0.0f,
		//1.0f, 1.0f,
		(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	0.0f,
		//1.0f, 1.0f,
		x*TILE_SIZE,		y*TILE_SIZE,		0.0f,
		//1.0f, 1.0f
	};
	//printf("sizeof(vertices): %d\n", sizeof(vertices));
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	
	while (1) {
		glx::poll();
		if (glx::done) {
			glx::clean_x();
			return 0;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.3, 0.3, 0.0f);
		
		glUseProgram(program);
		auto loc = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		auto projection_Location = glGetUniformLocation(program, "projection");
		//printf("\nLocation: %d\n", projection_Location);
		glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
			auto texLoc = glGetUniformLocation(program, "tex");
			glUniform1i(texLoc, texture);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glx::swap();
	}
	glx::clean_x();

	return 0;
}
