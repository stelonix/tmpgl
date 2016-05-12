#ifndef GENERATORS_H
#define GENERATORS_H
#include <assert.h>
#include <string.h>
#include <vector>

namespace gen {
	coord_grid vertex_grid(int w, int h, int l);
	coord_grid texture_map(game_tilemap tiles, asset_loader* a_loader);
	game_tilemap flatten_tilemap(map_tilemap tiles,	res_map<game_tileset> tilesets);
	res_map<game_tileset> flatten_tilesets(std::vector<string> tsets, asset_loader* a_loader);

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
}

#endif