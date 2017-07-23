#include <string.h>
#include "eng_texture.h"
#include "cfg.h"

using namespace cfg;


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

eng_texture eng_texture::blank_texture(int w, int h)
{
	unsigned int pbo;
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, w * h * 4 * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
		GLfloat* pData = nullptr;
		pData = (GLfloat*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
			memset(pData, 0x00, w * h * 4 * sizeof(GLfloat));
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
								 GL_UNSIGNED_BYTE, NULL);

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_FLOAT, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glDeleteBuffers(1, &pbo);
}

namespace engine {
	float normalize(int v, float unit)
	{
		return unit*v;
	}
}