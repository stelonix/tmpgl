#include "vbo.h"

vbo::vbo (std::vector<float> data) {
	setup_vao();
	setup_vbo(data);
	setup_pointers();
}

void vbo::setup_vao() {
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
}

void vbo::setup_vbo(std::vector<float> data) {
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(float),
		data.data(), GL_STATIC_DRAW);
}

void vbo::setup_pointers() {

}

