#include "aliases.hpp"
#include "debug.h"
#include "loader/assets.h"
#include "helpers/generators.h"
#include "engine/datatypes/game_map.h"
#include <cassert>
#include <vector>
#include <string.h>

#include "cfg.h"
using namespace cfg;
using namespace std;

coord_grid gen::sprite_vertex(std::vector<eng_sprite> sprs, int l)
{
	const int PER_VERTEX = 3;
	const int NUM_VERTEX = 6;
	const int NUM_ELEMENTS = NUM_VERTEX*PER_VERTEX;
	coord_grid retval; retval.resize(sprs.size() * NUM_ELEMENTS);
	auto z = l / 100.0f;
	int i = 0;
	printf("fdfdsf\n");
	for (auto it = sprs.begin(); it != sprs.end(); it ++)
	{
		auto spr = *it;
		printf("state %s frame %d\n", spr.state.c_str(), spr.frame);
		auto fr = spr.states[spr.state][spr.frame].coords;
		printf("--> %d\n", fr.x);
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wnarrowing"
		float vertices[] =
		{
			spr.x,			spr.y,			z + i / 100.0f,
			spr.x + fr.w,	spr.y,			z + i / 100.0f,
			spr.x + fr.w,	spr.y + fr.h,	z + i / 100.0f,
			spr.x,			spr.y,			z + i / 100.0f,
			spr.x,			spr.y + fr.h,	z + i / 100.0f,
			spr.x + fr.w,	spr.y + fr.h,	z + i / 100.0f,
		};
		#pragma GCC diagnostic pop
		for (int i = 0; i < sizeof(vertices)/sizeof(float); i++) vertices[i] *= cfg::MAG;
		memcpy(retval.data() + i * NUM_ELEMENTS,
				vertices, sizeof(vertices));
		i++;
	}
	return retval;
}

coord_grid gen::sprite_texture_map(std::vector<eng_sprite> sprs, tex_map& textures)
{
	coord_grid retval;
	int i = 0;
	for (auto it = sprs.begin(); it != sprs.end(); it++)
	{
		auto spr = *it;
		auto frame = spr.states[spr.state][spr.frame].coords;
		auto tex = textures[spr.states[spr.state][spr.frame].tex];
		//printf("-%s\n", frame.img.c_str());
		auto values = std::array<float, 12>(
		{
			tex.normalized_x[frame.x],				tex.normalized_y[frame.y],
			tex.normalized_x[frame.x + frame.w],	tex.normalized_y[frame.y],
			tex.normalized_x[frame.x + frame.w],	tex.normalized_y[frame.y + frame.h],
			tex.normalized_x[frame.x],				tex.normalized_y[frame.y],
			tex.normalized_x[frame.x],				tex.normalized_y[frame.y + frame.h],
			tex.normalized_x[frame.x + frame.w],	tex.normalized_y[frame.y + frame.h],
		});
		/*printf("%d u v w h: %d %d %d %d (%f %f %f %f)\n", tex->w, frame.u,frame.v,frame.w,frame.h,
			tex->normalized_x[frame.u],
			tex->normalized_y[frame.v],
			tex->normalized_x[frame.u + frame.w],
			tex->normalized_y[frame.v + frame.h]
			);*/

		retval.insert(retval.end(), values.begin(), values.end());
		i++;
	}
	return retval;
}

coord_grid gen::vertex_grid(int w, int h, int l)
{
	const int PER_VERTEX = 3;
	const int NUM_VERTEX = 6;
	const int NUM_ELEMENTS = NUM_VERTEX * PER_VERTEX;
	coord_grid retval; retval.resize(w * h * NUM_ELEMENTS);
	auto z = l / 100.0f;
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			float vertices[] =
			{
				x * TILE_SIZE,			y * TILE_SIZE,			z,
				(x + 1) * TILE_SIZE,	y * TILE_SIZE,			z,
				(x + 1) * TILE_SIZE,	(y + 1) * TILE_SIZE,	z,
				x * TILE_SIZE,			y * TILE_SIZE,			z,
				x * TILE_SIZE,			(y + 1) * TILE_SIZE,	z,
				(x + 1) * TILE_SIZE,	(y + 1) * TILE_SIZE,	z,
			};
			memcpy(retval.data() + (y * w + x) * NUM_ELEMENTS,
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

coord_grid gen::texture_map(game_tilemap tiles, loader* p_loader)
{
	coord_grid retval;
	int i = 0;
	for (auto it = tiles.begin(); it != tiles.end(); it++)
	{
		auto frame = (*it)[0];
		auto tex = p_loader->get_texture(frame.img);

		auto values = std::array<float, 12>(
		{
			tex.normalized_x[frame.u],		tex.normalized_y[frame.v],
			tex.normalized_x[frame.u + 1],	tex.normalized_y[frame.v],
			tex.normalized_x[frame.u + 1],	tex.normalized_y[frame.v + 1],
			tex.normalized_x[frame.u],		tex.normalized_y[frame.v],
			tex.normalized_x[frame.u],		tex.normalized_y[frame.v + 	1],
			tex.normalized_x[frame.u + 1],	tex.normalized_y[frame.v + 1],
		});
		retval.insert(retval.end(), values.begin(), values.end());
		i++;
	}
	return retval;
}

coord_grid gen::texview(eng_texture tex, int l)
{
	auto internal_w = (float)tex.internal_w;
	auto internal_h = (float)tex.internal_h;
	dbgprint("Texture view for texture %d (%f, %f)\n", tex.texture_id, internal_w, internal_h);
	auto z = l/100.0f;
	coord_grid retval;
	auto verts = std::vector<float>(
	{
		0,			0,			z,
		internal_w,	0,			z,
		internal_w,	internal_h,	z,
		0,			0,			z,
		0,			internal_h,	z,
		internal_w,	internal_h,	z,
	});
	auto t = tex;
	auto uvs = std::vector<float>(
	{
		t.normalize_u(0),			t.normalize_v(0),
		t.normalize_u(internal_w),	t.normalize_v(0),
		t.normalize_u(internal_w),	t.normalize_v(internal_h),
		t.normalize_u(0),			t.normalize_v(0),
		t.normalize_u(0),			t.normalize_v(internal_h),
		t.normalize_u(internal_w),	t.normalize_v(internal_h),
	});

	return intercalate<3,2>(verts,uvs);
}

res_map<game_tileset> gen::flatten_tilesets(
	std::vector<string> tsets, loader* p_loader)
{
	auto retval = res_map<game_tileset>();
	for (auto i = 0; i < tsets.size(); i++)
	{
		retval[i] = p_loader->get_tileset(tsets[i]);
	}
	return retval;
}
