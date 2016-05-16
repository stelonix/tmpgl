#ifndef VBO_H
#define VBO_H
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>

struct vbo {
	unsigned int vao_id;
	unsigned int vbo_id;

	vbo (std::vector<float> data);

	void setup_vao();

	void setup_vbo(std::vector<float> data);

	void setup_pointers();

};

#endif