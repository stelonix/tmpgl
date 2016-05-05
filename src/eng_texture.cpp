#include "eng_texture.h"

namespace engine {
	eng_texture::eng_texture(GLuint texture_id, int w, int h, int rw, int rh) {
		this->w = w;
		this->h = h;
		this->internal_w = rw;
		this->internal_h = rh;
		this->texture_id = texture_id;
		x_unit = 1.0f/rw;
		y_unit = 1.0f/rh;
	}

	float eng_texture::normalize_u(int u) {
		return normalize(u, x_unit);
	}

	float eng_texture::normalize_v(int v) {
		return normalize(v, y_unit);
	}

	float normalize(int v, float unit) {
		return unit*v;
	}
}