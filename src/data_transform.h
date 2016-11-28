#ifndef DATA_TRANSFORM_H
#define DATA_TRANSFORM_H
#include "game_map.h"


namespace data {
	game_tilemap flatten_layer(game_map map, int z);
}

#endif