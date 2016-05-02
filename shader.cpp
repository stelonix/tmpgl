#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "unwind.h"
#include "shader.h"


namespace scene {
	GLuint program;

	std::string get_shader_log(unsigned int shaderID) {
		std::vector<char> infoLog;
		GLint infoLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
		infoLog.resize(infoLen);

		glGetShaderInfoLog(shaderID, infoLen, &infoLen, &infoLog[0]);

		return std::string(infoLog.begin(), infoLog.end());
	}

	void add_shader(const char* shader, GLenum type) {
		GLuint s = glCreateShader (type);
		glShaderSource(s, 1, &shader, NULL);
		glCompileShader(s);
		int result;
		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("Shader fail\n%s\n",get_shader_log(s).c_str());
			
			backtrace();
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