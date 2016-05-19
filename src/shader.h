#ifndef SHADER_H
#define SHADER_H

#include "string"
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>

namespace scene {

	int get_attrib_loc(char const* attrib, GLuint program);
	int get_attrib_loc(const char* attrib, int program);
	int get_uniform_loc(const char* attrib, int program);
	string get_shader_log(unsigned int shaderID);
	
	struct shader_program {
		std::vector<GLuint> attached;
		GLuint program_id;
		shader_program();
		void new_scene();
		int uniform(string identifier);
		void add_shader(const char* shader, const char* fname, GLenum type);
		void link_shaders();
		void use_shaders();
		int attrib(string attrib);

	};

}
extern scene::shader_program* current_program;
#endif