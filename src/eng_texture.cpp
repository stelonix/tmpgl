#include "eng_texture.h"
#include "cfg.h"

using namespace cfg;

namespace engine {
	eng_texture::eng_texture()
	{

	}
	
	eng_texture::eng_texture(GLuint texture_id, int w, int h, int rw, int rh)
	{
		this->w = w;
		this->h = h;
		this->internal_w = rw;
		this->internal_h = rh;
		this->texture_id = texture_id;
		x_unit = 1.0f/rw;
		y_unit = 1.0f/rh;
		build_cache(cfg::ATILE);
	}

	float eng_texture::normalize_u(int u)
	{
		return engine::normalize(u, x_unit);
	}

	float eng_texture::normalize_v(int v)
	{
		return engine::normalize(v, y_unit);
	}

	void eng_texture::build_cache(int size)
	{
		printf("building cache tile\n");
		normalized_x.clear(); normalized_y.clear();
		for (int x = 0; x < (w / size * cfg::MAG) + 1; x++)
		{
			normalized_x[x] = normalize_u(x * size);
		}
		for (int y = 0; y < (h / size * cfg::MAG) + 1; y++)
		{
			normalized_y[y] = normalize_v(y * size);
		}
	}

	void eng_texture::build_cache(std::vector<sprite_frame> frames)
	{
		printf("building cache sprite\n");
		normalized_x.clear(); normalized_y.clear();
		for (auto it = frames.begin(); it != frames.end(); it++)
		{
			normalized_x[(*it).u] = normalize_u((*it).u);
			normalized_y[(*it).v] = normalize_v((*it).v);
			normalized_x[(*it).u + (*it).w] = normalize_u((*it).u + (*it).w);
			normalized_y[(*it).v + (*it).h] = normalize_v((*it).v + (*it).h);
		}
	}

	float normalize(int v, float unit)
	{
		return unit*v;
	}
}