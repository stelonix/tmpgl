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

	void new_scene() {
		program = glCreateProgram();
	}

	void link_shaders() {
		glLinkProgram(program);
	}

	void use_shaders() {
		glUseProgram(program);
	}

	int get_attrib_loc(const char* attrib) {
		return glGetAttribLocation(program, attrib);
	}

	int get_uniform_loc(const char* attrib) {
		return glGetUniformLocation(program, attrib);
	}
}