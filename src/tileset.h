#include <map>
#include <string>
#include <vector>

using string = std::string;

struct tile_frame {
	string img;
	int interval;
	string pass;
	int u,v;
};

typedef std::vector<tile_frame> game_tile;

struct tileset {
	string name;
	std::map<int, game_tile> tiles;
	static tileset from_json(string json);
};