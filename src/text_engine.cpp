#include "text_engine.h"

void text_engine::init() {
	
	if ((ft_error = FT_Init_FreeType (&ft_library)))
	  abort();
}

void text_engine::load_font(string filename, int size) {
	FT_Face ft_face;
	font new_font;
	if ((ft_error = FT_New_Face (ft_library, filename.c_str(), 0, &ft_face)))
	  abort();
	if ((ft_error = FT_Set_Char_Size (ft_face, size*64, size*64, 0, 0)))
	  abort();
	new_font.ft_face = ft_face;
	new_font.hb_font = hb_ft_font_create (ft_face, NULL);
	fonts[filename] = new_font;
}

eng_texture text_engine::draw_text(string font, string text) {
	auto cf = fonts[font];
	cf.render(text);
	return cf.upload_texture();
}
