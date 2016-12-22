#ifndef SHADER_H
#define SHADER_H

#include "string"
#include "vbo.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>

template <typename T>
float* glm_addr(T* ptr) {
	return static_cast<float*>(glm::value_ptr(*ptr));
}

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
		void draw(vbo& target);

		// uniforms

		void uniform(string uni, std::vector<glm::vec3> value);
		void uniform(string uni, glm::vec3 value);
		void uniform(string uni, std::vector<glm::mat4> values, GLenum transpose = GL_FALSE);
		void uniform(string uni, glm::mat4 value, GLenum transpose = GL_FALSE);

		
	};

}
extern scene::shader_program* current_program;

struct glsl_type_set {
  GLenum      type;
  const char* name;
};

extern glsl_type_set type_set[];

#endif