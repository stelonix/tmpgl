#ifndef ENG_SPRITE_H
#define ENG_SPRITE_H

#include <GL/glew.h>
#include <GL/gl.h>
#include "eng_texture.h"
#include "eng_types.h"
#include "string"
#include "game_sprite.h"

struct eng_sprite {
	int x,y,z;
	string state;
	std::map<string, std::vector<quad_coords>> states;
	int frame;
	int cur_tick;
	eng_texture tex;

	bool tick(int val);
	eng_sprite();
};



#endif