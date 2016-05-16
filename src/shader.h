#ifndef SHADER_H
#define SHADER_H

#include "string"
#include <GL/glew.h>
#include <GL/gl.h>

namespace scene {

	int get_attrib_loc(char const* attrib, GLuint program);
	int get_attrib_loc(const char* attrib, int program);
	int get_uniform_loc(const char* attrib, int program);
	std::string get_shader_log(unsigned int shaderID);
	
	struct shader_program {
		GLuint program_id;
		shader_program();
		void new_scene();
		int uniform(std::string identifier);
		void add_shader(const char* shader, const char* fname, GLenum type);
		void link_shaders();
		void use_shaders();
		int attrib(std::string attrib);

	};

}
extern scene::shader_program* current_program;
#endif