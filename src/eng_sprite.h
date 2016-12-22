#ifndef ENG_SPRITE_H
#define ENG_SPRITE_H
#include <GL/gl.h>
#include "string"
#include "game_sprite.h"

struct eng_sprite {
	int x,y,z;
	string state;
	game_sprite* spr;
	int frame;
	int cur_tick;

	bool tick(int val);
	eng_sprite();
};

struct eng_texel {
	GLint tex;
	int u, v, w, h;
};

#endif