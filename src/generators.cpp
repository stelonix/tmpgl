#include "generators.h"
#include <vector>
#include <string.h>

#include "cfg.h"
using namespace cfg;

std::vector<float> gen::vertex_grid(int w, int h, int l) {
	const int PER_VERTEX = 3;
	const int NUM_VERTEX = 6;
	const int NUM_ELEMENTS = NUM_VERTEX*PER_VERTEX;
	std::vector<float> retval; retval.resize(w*h*NUM_ELEMENTS);
	auto z = l/100.0f;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			float vertices[] ={
				x*TILE_SIZE,		y*TILE_SIZE,		z,
				(x+1)*TILE_SIZE,	y*TILE_SIZE,		z,
				(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	z,
				x*TILE_SIZE,		y*TILE_SIZE,		z,
				x*TILE_SIZE,		(y+1)*TILE_SIZE,	z,
				(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	z,
			};
			memcpy(retval.data()+(y*w+x)*NUM_ELEMENTS,
					vertices, sizeof(vertices));
		}
	}
	return retval;	
}
/*
std::vector<float>
generators::texture_map(tilemap tiles,
			std::map<int,
				tuple<tileset, eng_texture>
			> tile_data)
{
	const int PER_COORD = 2;
	const int NUM_COORD = 6;
	const int NUM_ELEMENTS = PER_COORD*NUM_COORD;
	std::vector<float> retval; retval.resize(tiles.size()*NUM_ELEMENTS);
	for (auto it = tiles.begin(); it != tiles.end(); it++) {
		auto cur_tile = get<0>(tile_data).tiles[get<1>(*it)];
		auto t = get<1>(tile_data);
		a_loader->loaded_tex[]
		int tx = cur_tile.u; int ty = cur_tile.v;
		float norm_texels[] ={
			t.normalize_u(tx*ATILE), t.normalize_v(ty*ATILE),
			t.normalize_u((tx+1)*ATILE), t.normalize_v(ty*ATILE),
			t.normalize_u((tx+1)*ATILE), t.normalize_v((ty+1)*ATILE),
			t.normalize_u(tx*ATILE), t.normalize_v(ty*ATILE),
			t.normalize_u(tx*ATILE), t.normalize_v((ty+1)*ATILE),
			t.normalize_u((tx+1)*ATILE), t.normalize_v((ty+1)*ATILE)
		};
		memcpy(retval.data()+(y*w+x)*NUM_ELEMENTS,
				norm_texels, sizeof(norm_texels));
	}
	return retval;
}*/