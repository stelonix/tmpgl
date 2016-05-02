#include <map>
#include <string>
#include <vector>

struct tile_frame {
	std:: string img;
	int interval;
	int passability;
	int u,v;
};

typedef std::vector<tile_frame> game_tile;

struct tileset {
	std::string name;
	std::map<int, game_tile> tiles;
};