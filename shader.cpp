#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "shader.h"


namespace scene {
	GLuint program;
	
	void add_shader(const char* shader, GLenum type) {
		GLuint s = glCreateShader (type);
		glShaderSource(s, 1, &shader, NULL);
		glCompileShader(s);
		int result;
		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("Shader fail\n");
			exit(1);
		}
		glAttachShader(program, s);
	}
}