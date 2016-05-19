#ifndef ENG_TEXTURE_H
#define ENG_TEXTURE_H

#include "aliases.hpp"
#include <vector>
#include <GL/glew.h>
#include <GL/gl.h>

namespace engine {
	float normalize(int v, float unit);

	struct eng_texture {
		std::vector<float> normalized_x;
		std::vector<float> normalized_y;
		int w, h;
		int internal_w, internal_h;
		float x_unit, y_unit;
		GLuint texture_id;

		eng_texture(GLuint texture_id, int w, int h, int rw, int rh);
		eng_texture();
		float normalize_u(int u);
		float normalize_v(int v);
		void build_cache();
	};
}

#endif