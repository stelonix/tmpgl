#include "string"
#include <map>
#include <vector>

struct game_tile_frame {
	string img;
	int interval;
	string pass;
	int u,v;
};

typedef std::vector<game_tile_frame> game_tile;

struct game_tileset {
	string name;
	std::map<int, game_tile> tiles;
	static game_tileset from_json(string json);
};