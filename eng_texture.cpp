#include <GL/gl.h>
#include <tuple>



eng_texture::eng_texture(GLuint texture_id, int w, int h) {
	this->w = w;
	this->h = h;
	this->texture_id = texture_id;
	x_unit = 1.0f/w;
	y_unit = 1.0f/h;
}

std::tuple<int,int> eng_texture::normalize(int u, int v) {
	return std::make_tuple(x_unit*u, y_unit*v)
}