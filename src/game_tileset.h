#ifndef GAME_TILESET_H
#define GAME_TILESET_H

#include "aliases.hpp"
#include "string"
#include <map>
#include <vector>

struct game_tile_frame {
	string img;
	int interval;
	string pass;
	int u,v;
};

struct game_tileset {
	string name;
	std::map<int, game_tile> tiles;
	static game_tileset from_json(string json);
};

#endif