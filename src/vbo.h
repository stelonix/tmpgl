#ifndef VBO_H
#define VBO_H
#include "string"
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

struct vbo;

struct vap_desc {
	string attr; size_t num; GLenum type;
};
struct vao_pointer {
	
	std::vector<vap_desc> pointers;

	vao_pointer add_pointer(string attrib, size_t num_elems, GLenum type);
	size_t get_size(GLenum type);
	void attach(vbo targ);
};

struct vbo {
	unsigned int vao_id;
	unsigned int vbo_id;

	vbo (std::vector<float> data);
	vbo ();
	void setup_vao();
	void buffer_data(std::vector<float> data);

	vao_pointer add_pointer(string attrib, size_t num_elems, GLenum type);
};

#endif