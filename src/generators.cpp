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
	coord_grid retval; retval.resize(w*h*NUM_ELEMENTS);
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

game_tilemap gen::flatten_tilemap(map_tilemap tiles,
	res_map<game_tileset> tilesets)
{
	game_tilemap retval;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		retval.push_back(tilesets[get<0>(*it)].tiles[get<1>(*it)]);
	}
	return retval;	
}

coord_grid gen::texture_map(game_tilemap tiles, asset_loader* a_loader)
{
	coord_grid retval;
	//printf("%d\n", tiles.size());
	int i = 0;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{

		//printf("%d\n", (*it).size());
		auto frame = (*it)[0];
		
		//exit(0);
		printf("%s\n", frame.img.c_str());
		auto tex = a_loader->loaded_tex[ASSETS_DIR+frame.img];
		auto values = std::array<float, 12>(
		{
			frame.u, frame.v,
			frame.u+1, frame.v,
			frame.u+1, frame.v+1,
			frame.u, frame.v,
			frame.u, frame.v+1,
			frame.u+1, frame.v+1,
		});
		for (auto i = 0; i < values.size(); i+=2) {
			auto nibs = tex.normalize(values[i], values[i+1]);
			retval.push_back(nibs[0]);
			retval.push_back(nibs[1]);
		}
		
		printf("adding %d\n", i);
		i++;
	}
	return retval;
}

res_map<game_tileset> gen::flatten_tilesets(
	std::vector<string> tsets, asset_loader* a_loader)
{
	auto retval = res_map<game_tileset>();
	for (auto i = 0; i < tsets.size(); i++)
	{
		retval[i] = a_loader->loaded_tilesets[ASSETS_DIR+tsets[i]];
		printf("%s is %s\n", (ASSETS_DIR+tsets[i]).c_str(), retval[i].name.c_str());
		//exit(0);
	}
	return retval;
}
