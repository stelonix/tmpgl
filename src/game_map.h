#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "aliases.hpp"
#include "string"
#include <tuple>
#include <vector>
#include "game_tileset.h"

struct map_decal
{
	string name;
	string sprite;
	int x;
	int y;
	int z;
};

struct map_layer
{
	string name;
	std::vector<map_decal> objects;
	map_tilemap tiles;
	int z;
};

struct game_map
{
	// Attrs
	
	int w, h;
	std::vector<map_layer> layers;
	string name;
	std::vector<string> tilesets;

	// Constructors
	game_map();
	game_map(string name, int w, int h, int l);

	// Required
	void resize(int w, int h);
	void set_layers(int l);

	// ?
	void set_tile(int layer, int x, int y, map_tile tile);
	
	// Data format
	game_tilemap flatten_layer(int z);
	map_tile get_tile(int layer, int x, int y);

	// Real constructor
	static game_map from_json(string json);
};

#endif