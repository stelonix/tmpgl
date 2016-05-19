#ifndef FONTS_H
#define FONTS_H
#include <vector>
#include "string"
#include "eng_texture.h"

using eng_texture = engine::eng_texture;

struct font {
	struct char_info_t
	{
		int x;
		int y;
		int width;
		int height;
		int left;
		int top;
		int advance;
		int row;
		float uv[4][2];
		unsigned char* bitmap;
	};

	struct font_info_t
	{
		int max_height;
		std::map<int, font::char_info_t> ch;
	};
	font_info_t    info;
	eng_texture    texture;

	void load();
	void fill_texture_data(unsigned int ch,
		font::font_info_t* font, unsigned int texture_width,
		unsigned char* texture_data, int char_x, int char_y);
	string to_json();
};


struct text_buffer
{
	coord_grid set(string str, font* fnt);
};

#endif