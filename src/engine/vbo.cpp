#include "shader.h"
#include "vbo.h"


vbo::vbo (std::vector<float> data) : usage(GL_INVALID_VALUE) {
	setup_vao();
	buffer(data);

}

vbo::vbo() : usage(GL_INVALID_VALUE)
{

}

void vbo::init(GLenum usage) {
	setup_vao();
	num_els = 0;
	this->usage = usage;
}

void vbo::setup_vao() {
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
}

void vbo::buffer(std::vector<float> data) {
	num_els = data.size();
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	GLenum buffer_type;
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(float),
		data.data(), usage);
}

void vbo::buffer(std::vector<float> data, int offset) {
	num_els = data.size();
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	GLenum buffer_type;
	glBufferSubData(GL_ARRAY_BUFFER,
		offset,
		data.size() * sizeof(float),
		data.data());
}

vao_pointer vao_pointer::add_pointer(string attrib, size_t num_elems, GLenum type) {
	pointers.push_back({attrib,num_elems,type});
	return *this;
}

vao_pointer vbo::add_pointer(string attrib, size_t num_elems, GLenum type) {
	vao_pointer vap;
	return vap.add_pointer(attrib, num_elems, type);
}

size_t vao_pointer::get_size(GLenum type) {
	switch (type) {
		case GL_FLOAT: return sizeof(float);
	}
	return 0;
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
void vao_pointer::attach(vbo targ) {
	int offset = 0; int total_size = 0;
	for (auto it = pointers.begin(); it != pointers.end(); it++)
	{
		total_size += (*it).num * get_size((*it).type);
	}
	for (auto it = pointers.begin(); it != pointers.end(); it++)
	{
		auto loc = scene::get_attrib_loc((*it).attr.c_str(), current_program->program_id);
			glEnableVertexAttribArray(loc);
			glVertexAttribPointer(loc, (*it).num, (*it).type, GL_FALSE, total_size, (void*)offset);
		offset += get_size((*it).type) * (*it).num;
	}
	targ.pointers = *this;
}
#pragma GCC diagnostic pop

void vbo::draw()
{
	if (usage == GL_INVALID_VALUE) return;
	glBindVertexArray(vao_id);
	glDrawArrays(GL_TRIANGLES, 0, num_els);
}