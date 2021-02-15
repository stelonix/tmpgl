#ifndef TEXT_ENGINE_H
#define TEXT_ENGINE_H

#include "helpers/string_helper"
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "engine/text/fonts.h"

struct text_engine {
	FT_Library ft_library;
	FT_Error ft_error;
	std::map<string, font> fonts;

	eng_texture draw_text(string font, string text);
	void init();
	void render();
	void load_font(string filename, int size);
};

#endif