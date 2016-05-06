#include "string"
#include <vector>
#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "unwind.h"

namespace scene {
	
	int get_attrib_loc(const char* attrib, GLuint program) {
		return glGetAttribLocation(program, attrib);
	}

	int get_uniform_loc(const char* attrib, GLuint program) {
		return glGetUniformLocation(program, attrib);
	}

	shader_program::shader_program() {
		new_scene();
	}

	void shader_program::new_scene() {
		program_id = glCreateProgram();
	}

	int shader_program::uniform(string identifier) {
		return get_uniform_loc(identifier.c_str(), program_id);
	}

	void shader_program::add_shader(const char* shader, const char* fname, GLenum type) {
		GLuint s = glCreateShader (type);
		glShaderSource(s, 1, &shader, NULL);
		glCompileShader(s);
		int result;
		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("Error compiling %s\n%s\n",fname,
				get_shader_log(s).c_str());
			
			backtrace();
			exit(1);
		}
		glAttachShader(program_id, s);
	}
	void shader_program::link_shaders() {
		glLinkProgram(program_id);
	}

	void shader_program::use_shaders() {
		glUseProgram(program_id);
	}

	int shader_program::attrib(string attrib) {
		return get_attrib_loc(attrib.c_str(), program_id);
	}

	string get_shader_log(unsigned int shaderID) {
		std::vector<char> infoLog;
		GLint infoLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
		infoLog.resize(infoLen);

		glGetShaderInfoLog(shaderID, infoLen, &infoLen, &infoLog[0]);

		return std::string(infoLog.begin(), infoLog.end());
	}

	

}