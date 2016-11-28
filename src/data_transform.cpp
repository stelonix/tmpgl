#include <tuple>
#include <vector>
#include "game_map.h"
#include "generators.h"
#include "loader.h"

using namespace std;
extern loader p_loader;
namespace data {
	game_tilemap flatten_layer(game_map map, int z) {
		
		auto tiles = map.layers[z].tiles;

		auto retval = game_tilemap();
		auto tmaps = gen::flatten_tilesets(map.tilesets, &p_loader);
		for (auto i = 0; i < tiles.size(); i++) {
			retval.push_back(	tmaps[get<0>(tiles[i])].tiles
									 [get<1>(tiles[i])]);
		}
		return retval;
	}
}