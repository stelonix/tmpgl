#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include "aliases.hpp"
#include "string"
#include <map>
#include <vector>

struct hitbox {
	int x,y,w,h;
};

struct sprite_frame {
	string img;
	int u,v,w,h,ox,oy;
	int interval;
	std::vector<hitbox> hitboxes;
};

struct game_sprite {
	int id;
	string name;
	std::map<string, std::vector<sprite_frame>> states;
	
	static game_sprite from_json(string j);
	std::vector<sprite_frame> get_frame_array();
};

#endif