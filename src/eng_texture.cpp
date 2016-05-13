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
		build_cache();
	}

	float eng_texture::normalize_u(int u)
	{
		return engine::normalize(u, x_unit);
	}

	float eng_texture::normalize_v(int v)
	{
		return engine::normalize(v, y_unit);
	}

	void eng_texture::build_cache()
	{
		for (int x = 0; x < (w / TILE_SIZE) + 1; x++)
		{
			normalized_x.push_back(normalize_u(x*ATILE));
		}
		for (int y = 0; y < (h / TILE_SIZE) + 1; y++)
		{
			normalized_y.push_back(normalize_v(y*ATILE));
			//printf("%f -> %f \n", y*ATILE, normalize_v(y*ATILE));
		}
	}

	float normalize(int v, float unit)
	{
		return unit*v;
	}
}