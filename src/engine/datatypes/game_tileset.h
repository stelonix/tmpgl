#ifndef GAME_TILESET_H
#define GAME_TILESET_H

#include "aliases.hpp"
#include "helpers/string"
#include <map>
#include <vector>

typedef int tileid_t;

struct game_tile_frame {
	string img;
	int interval;
	string pass;
	int u,v;
};

struct game_tileset {
	string name;
	string path;
	std::map<tileid_t, game_tile> tiles;
	static game_tileset from_json(string json, string path);
};

#endif