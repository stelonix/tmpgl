#ifndef FONTS_H
#define FONTS_H
#include <vector>
#include "string"
#include "eng_texture.h"
#include <hb.h>
#include <hb-ft.h>
#include <cairo.h>
#include <cairo-ft.h>
#include <tuple>

typedef std::tuple<int, int> wh_size_t;

struct font {
	// Logical fonts
	cairo_font_face_t* 		cairo_font;
	FT_Face					ft_face;
	hb_font_t* 				hb_font;
	
	// Cairo
	cairo_surface_t*		cairo_surface;
	cairo_t*				cr;

	// HB
	hb_glyph_info_t*		infos;
	hb_glyph_position_t*	pos;
	
	// Constructors
	font();
	font(FT_Face ft, hb_font_t* hb, cairo_font_face_t* cf);

	// Functions
	cairo_glyph_t* build_glyphs(int len);
	static wh_size_t calc_surface_area(
		hb_direction_t dir,
		hb_glyph_position_t* pos,
		unsigned int len
	);
	void 				init_cairo_font();
	void 				render(string text);
	void				render_nowrap(string text);
	double 				setup_baseline(hb_direction_t dir);
	cairo_surface_t* 	setup_cairo_surface(int w, int h);
	hb_buffer_t* 		shape(string text);
	string 				to_json();
	eng_texture 		upload_texture();
};


#endif