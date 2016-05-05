#include <map>
#include <string>
#include <vector>

struct tile_frame {
	std::string img;
	int interval;
	std::string pass;
	int u,v;
};

typedef std::vector<tile_frame> game_tile;

struct tileset {
	std::string name;
	std::map<int, game_tile> tiles;
	static tileset from_json(std::string json);
};