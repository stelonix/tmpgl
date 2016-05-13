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
		//retval.resize(verts.size() + uvs.size());
		//printf("intercalate size: %d\n", retval.size());
		auto sz = (N+M)*sizeof(float);
		for (int i = 0; i < verts.size() / N; i++)
		{
			// verts
			retval.push_back(verts[i*N]);
			retval.push_back(verts[i*N+1]);
			retval.push_back(verts[i*N+2]);
			
			// uv
			retval.push_back(uvs[i*M]);
			retval.push_back(uvs[i*M+1]);
			
			// printf("x,y,z: %f,%f,%f | u,v: %f %f\n",
			// 	retval[i],
			// 	retval[i+1],
			// 	retval[i+2],
			// 	retval[i+3],
			// 	retval[i+4]);
		}
		//exit(0);
		return retval;
	}
}

#endif