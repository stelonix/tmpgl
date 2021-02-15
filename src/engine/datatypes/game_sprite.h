#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

#include "aliases.h"
#include <nlohmann/json.hpp>
#include <map>
#include <vector>

struct hitbox {
	int x,y,w,h;
};

struct sprite_frame {
	int index;
	string img;
	int u,v,w,h,ox,oy;
	int interval;
	std::vector<hitbox> hitboxes;
};

struct game_sprite {
	int id;
	string name;
	string path;
	std::map<string, std::vector<sprite_frame>> states;
	static game_sprite from_json(string j, string path);
	static game_sprite from_json(nlohmann::json j, string path);
	std::vector<sprite_frame> get_frame_array();
};

#endif