#ifndef VBO_H
#define VBO_H
#include "helpers/string"
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

struct vbo;

struct vap_desc {
	string attr;
	size_t num;
	GLenum type;
};

struct vao_pointer {
	std::vector<vap_desc> pointers;
	vao_pointer add_pointer(string attrib, size_t num_elems, GLenum type);
	void attach(vbo targ);
	size_t get_size(GLenum type);
};

struct vbo {
	// Data
	std::vector<float>* _stream_cache;
	unsigned int num_els;
	int max_size;
	vao_pointer pointers;
	GLenum usage;
	unsigned int vao_id;
	unsigned int vbo_id;
	
	// Constructors
	vbo (std::vector<float> data);
	vbo ();

	// Functions
	void buffer(std::vector<float> data);
	void buffer(std::vector<float> data, int offset);
	void draw();
	void init(GLenum usage = GL_STATIC_DRAW);
	void setup_vao();
	
	// VAO handling
	vao_pointer add_pointer(string attrib, size_t num_elems, GLenum type);
};

#endif