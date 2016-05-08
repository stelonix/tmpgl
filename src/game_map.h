#include "string"
#include <tuple>
#include <vector>
#include "eng_texture.h"
#include "game_tileset.h"

using eng_texture = engine::eng_texture;
typedef std::tuple<int, int> map_tile;
typedef std::vector<map_tile> map_tilemap;

struct map_decal {
	string name;
	string sprite;
	int x;
	int y;
	int z;
};

struct map_layer {
	string name;
	std::vector<map_decal> objects;
	map_tilemap tiles;
	int z;
};

struct game_map {
	string name;
	int w;
	int h;
	std::vector<map_layer> layers;
	std::vector<string> tilesets;

	game_map(string name, int w, int h, int l);

	void set_layers(int l);

	void set_tile(int layer, int x, int y, map_tile tile);

	void resize(int w, int h);

	map_tile get_tile(int layer, int x, int y);

	static game_map from_json(string json);
};