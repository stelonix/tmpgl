#include <nlohmann/json.hpp>
#include "debug.h"
#include "engine/text/fonts.h"
#include "helpers/generators.h"

string font::to_json()
{
		return "";
}
#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)

cairo_glyph_t* font::build_glyphs(int len)
{
	auto cairo_glyphs = cairo_glyph_allocate (len);
	double current_x = 0;
	double current_y = 0;
	for (unsigned int i = 0; i < len; i++)
	{
	  cairo_glyphs[i].index = infos[i].codepoint;
	  cairo_glyphs[i].x = current_x + pos[i].x_offset / 64.;
	  cairo_glyphs[i].y = -(current_y + pos[i].y_offset / 64.);
	  current_x += pos[i].x_advance / 64.;
	  current_y += pos[i].y_advance / 64.;
	}
	return cairo_glyphs;
}

wh_size_t
font::calc_surface_area(hb_direction_t dir, hb_glyph_position_t* pos, unsigned int len)
{
	/* Draw, using cairo. */
	double width = 2 * MARGIN;
	double height = 2 * MARGIN;
	for (unsigned int i = 0; i < len; i++)
	{
	  width  += pos[i].x_advance / 64.;
	  height -= pos[i].y_advance / 64.;
	}
	if (HB_DIRECTION_IS_HORIZONTAL (dir))
	  height += FONT_SIZE;
	else
	  width  += FONT_SIZE;
	return make_tuple(ceil(width), ceil(height));
}

font::font()
{

}

font::font(FT_Face ft, hb_font_t* hb, cairo_font_face_t* cf)
{
	hb_font = hb;
	cairo_font = cf;
	ft_face = ft;
}

void font::init_cairo_font()
{
	/* Set up cairo font face. */

	cairo_font = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
	cairo_set_font_face (cr, cairo_font);
	cairo_set_font_size (cr, FONT_SIZE);

	cairo_font_options_t *cfo = cairo_font_options_create();
	cairo_get_font_options(cr, cfo);
	cairo_font_options_set_antialias(cfo, CAIRO_ANTIALIAS_NONE);
	cairo_set_font_options (cr, cfo);
	cairo_font_options_destroy(cfo);
}

void font::render(string text)
{
	auto hb_buffer = shape(text);
	auto direction = hb_buffer_get_direction(hb_buffer);

	unsigned int len = hb_buffer_get_length (hb_buffer);
	infos = hb_buffer_get_glyph_infos (hb_buffer, NULL);
	pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);

	auto s_dim = calc_surface_area(direction, pos, len);
	setup_cairo_surface(get<0>(s_dim), get<1>(s_dim));
	init_cairo_font();

	setup_baseline(direction);
	auto glyphs = build_glyphs(len);
		cairo_show_glyphs (cr, glyphs, len);
	cairo_glyph_free (glyphs);
	hb_buffer_destroy(hb_buffer);
}

void font::render_nowrap(string text)
{
	auto hb_buffer = shape(text);
	auto direction = hb_buffer_get_direction(hb_buffer);

	unsigned int len = hb_buffer_get_length (hb_buffer);
	infos = hb_buffer_get_glyph_infos (hb_buffer, NULL);
	pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);

	auto s_dim = calc_surface_area(direction, pos, len);
	setup_cairo_surface(get<0>(s_dim), get<1>(s_dim));
	init_cairo_font();
	setup_baseline(direction);
	auto glyphs = build_glyphs(len);
		cairo_show_glyphs (cr, glyphs, len);
	cairo_glyph_free (glyphs);
}

double font::setup_baseline(hb_direction_t dir)
{
	double baseline = 0.0f;
	if (HB_DIRECTION_IS_HORIZONTAL(dir))
	{
	  cairo_font_extents_t font_extents;
	  cairo_font_extents (cr, &font_extents);

	  baseline = (FONT_SIZE - font_extents.height) * .5 + font_extents.ascent;
	  cairo_translate (cr, 0, baseline);
	}
	else
	{
	  cairo_translate (cr, FONT_SIZE * .5, 0);
	}
	return baseline;
}

cairo_surface_t* font::setup_cairo_surface(int w, int h)
{
	/* Set up cairo surface. */
	cairo_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      w, h);
	cr = cairo_create (cairo_surface);
	// last one is alpha
	cairo_set_source_rgba (cr, 1., 1., 1., 1.);
	cairo_paint (cr);
	cairo_set_source_rgba (cr, 0., 0., 0., 1.);
	cairo_translate (cr, MARGIN, MARGIN);
	//printf("surface %x context %x w,h: %d, %d\n", cairo_surface, cr, w, h);
	return cairo_surface;
}

hb_buffer_t* font::shape(string text)
{
	auto hb_buffer = hb_buffer_create ();
	hb_buffer_add_utf8 (hb_buffer, text.c_str(), -1, 0, -1);
	hb_buffer_guess_segment_properties (hb_buffer);

	/* Shape it! */
	hb_shape (hb_font, hb_buffer, NULL, 0);
	return hb_buffer;
}

eng_texture font::upload_texture()
{
	unsigned char* data = cairo_image_surface_get_data(cairo_surface);
	int tex_w = cairo_image_surface_get_width(cairo_surface);
	int tex_h = cairo_image_surface_get_height(cairo_surface);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_w, tex_h, 0, GL_RGBA,
	             GL_UNSIGNED_BYTE, data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	auto retval = eng_texture(texture, tex_w, tex_h, tex_w, tex_h);
	return retval;
}

