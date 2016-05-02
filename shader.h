#include <GL/gl.h>

namespace scene {
	void add_shader(const char* shader, GLenum type);
	void new_scene();
	void link_shaders();
	void use_shaders();
	int get_attrib_loc(const char* attrib);
	int get_uniform_loc(const char* attrib);
}