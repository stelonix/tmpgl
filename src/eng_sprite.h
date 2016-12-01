#ifndef ENG_SPRITE_H
#define ENG_SPRITE_H
#include "string"
#include "game_sprite.h"

struct eng_sprite {
	int x,y,z;
	string state;
	game_sprite* spr;
	int frame;
};

#endif