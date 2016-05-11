#include "aliases.hpp"
#include "assets.h"
#include "generators.h"
#include "game_map.h"
#include <cassert>
#include <vector>
#include <string.h>

#include "cfg.h"
using namespace cfg;
using namespace std;

coord_grid gen::vertex_grid(int w, int h, int l)
{
	const int PER_VERTEX = 3;
	const int NUM_VERTEX = 6;
	const int NUM_ELEMENTS = NUM_VERTEX*PER_VERTEX;
	std::vector<float> retval; retval.resize(w*h*NUM_ELEMENTS);
	auto z = l/100.0f;
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			float vertices[] =
			{
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

game_tilemap flatten_tilemap(map_tilemap tiles,
	res_map<game_tileset> tilesets)
{
	game_tilemap retval;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		retval.push_back(tilesets[get<0>(*it)].tiles[get<1>(*it)]);
	}
	return retval;	
}

coord_grid texture_map(game_tilemap tiles, asset_loader* a_loader)
{
	coord_grid retval;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		auto frame = (*it)[0];
		auto nibs = a_loader->loaded_tex[frame.img]
			.normalize(frame.u, frame.v);
		retval.push_back(nibs[0]);
		retval.push_back(nibs[1]);
	}
	return retval;
}

template <int N, int M> coord_grid intercalate(coord_grid verts, coord_grid uvs)
{
	assert(verts.size() / N == uvs.size() / M);
	coord_grid retval;
	retval.reserve(verts.size() + uvs.size());
	auto sz = (N+M)*sizeof(float);
	for (int i = 0; i < verts.size() / N; i++)
	{
		memcpy(retval.data()+(i)*sz,
			verts.data()+i*N*sizeof(float), N);
		memcpy(retval.data()+(i)*sz+N*sizeof(float),
			uvs.data()+i*M*sizeof(float), M);
	}
	return retval;
}