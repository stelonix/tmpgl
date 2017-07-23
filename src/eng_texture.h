#ifndef ENG_TEXTURE_H
#define ENG_TEXTURE_H

#include <GL/glew.h>
#include <GL/gl.h>
#include "aliases.hpp"
#include "game_sprite.h"
#include <vector>



namespace engine {
	float normalize(int v, float unit);

	
}

struct eng_texture {
	std::map<int, float> normalized_x;
	std::map<int, float> normalized_y;
	int w, h;
	int internal_w, internal_h;
	float x_unit, y_unit;
	GLuint texture_id;

	eng_texture(GLuint texture_id, int w, int h, int rw, int rh);
	eng_texture();
	float normalize_u(int u);
	float normalize_v(int v);
	float normalize_s(int s);
	float normalize_t(int t);
	void build_cache(int size);
	void build_cache(std::vector<sprite_frame> frames);
	static eng_texture blank_texture(int w, int h);
};

#endif