#ifndef ENG_TEXTURE_H
#define ENG_TEXTURE_H

#include "include/GL/gl3w.h"
#include <GL/gl.h>
#include "aliases.h"
#include "engine/datatypes/game_sprite.h"
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
	void blit(eng_texture dst, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y);
	static eng_texture blank_texture(int w, int h);
};

#endif
