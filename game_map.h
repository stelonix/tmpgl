#include <vector>
#include <tuple>
#include <string>
#include "tileset.h"

typedef std::tuple<int, int> tile_t;
typedef std::vector<tile_t> tilemap;

struct map_decal {
	std::string name;
	std::string sprite;
	int x;
	int y;
	int z;
};
struct map_layer {
	std::string name;
	std::vector<map_decal> objects;
	tilemap tiles;
	int z;
};



struct game_map {
	std::string name;
	int w;
	int h;
	std::vector<map_layer> layers;
	std::vector<std::string> tilesets;

	game_map(std::string name, int w, int h, int l);

	void set_layers(int l);

	void set_tile(int layer, int x, int y, tile_t tile);

	void resize(int w, int h);

	tile_t get_tile(int layer, int x, int y);

	static game_map from_json(std::string json);
};