#include "string"
#include <iostream>
#include <map>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <dirent.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include "include/json.hpp"
//#include "include/prettyprint.hpp"
#include "assets.h"
#include "cfg.h"
#include "boilerplate.h"
#include "debug.h"
#include "engine.h"
#include "fonts.h"
#include "game_map.h"
#include "generators.h"
#include "game_sprite.h"
#include "helpers.h"
#include "shader.h"
#include "vbo.h"
#include <hb.h>
#include <hb-ft.h>
#include <cairo.h>
#include <cairo-ft.h>

#define FONT "Acme 5 Compressed Caps Outline Xtnd"
#define TEXT "The quick brown fox is so かわいい!"
using namespace cfg;
using json = nlohmann::json;
using shader_program = scene::shader_program;

asset_loader* a_loader;
shader_program* current_program;
std::map<int, int> keys;

glm::mat4 pan;
float panx, pany;
coord_grid vertex_data;

void init_crt() {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
}

void pan_view(float x, float y) {
	pan = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}
vbo texture_viewer;
game_engine* eng;
#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)
unsigned int
create_texture (unsigned int width,
                unsigned int height,
                unsigned char *pixels)
{
    unsigned int texture_id;

    glGenTextures (1, &texture_id);
    glBindTexture (GL_TEXTURE_2D, texture_id);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D (GL_TEXTURE_2D,
                  0,
                  GL_RGBA,
                  width,
                  height,
                  0,
                  GL_BGRA,
                  GL_UNSIGNED_BYTE,
                  pixels);

    return texture_id;
}

int main(int argc, char *argv[]) {
	eng = new game_engine();
	init_crt();
	
	
	

	
	font f;


	a_loader = new asset_loader();
	auto mymap = game_map::from_json(read_file<string>(ASSETS_DIR+"map.json"));
	a_loader->load_tileset(ASSETS_DIR+"tileset.json");
	game_sprite::from_json(read_file<string>(ASSETS_DIR+"sprite.json"));
	
	eng->load_shaders();
	glx::setup_x(HORZ_RES, VERT_RES);
	//f.load();
	auto t = a_loader->load_texture(ASSETS_DIR+"indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	
	glx::init_glew();
	glx::init_gl(HORZ_RES, VERT_RES);
	auto projection = glm::ortho( 0.f, float(HORZ_RES), float(VERT_RES), 0.0f, 0.0f, 100.f ); 
	glm::mat4 VP = glm::mat4();
	glm::mat4 view = glm::lookAt(
				glm::vec3(0,0,1), // Camera is at (0,0,5), in World Space
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	VP = projection*view;
	auto sp = shader_program();
		sp.add_shader(a_loader->shader_lib["basic.glsl"].c_str(), "basic.glsl", GL_VERTEX_SHADER);
		sp.add_shader(a_loader->shader_lib["frag.glsl"].c_str(), "frag.glsl", GL_FRAGMENT_SHADER);
		sp.link_shaders();
		sp.use_shaders();
	auto wp = shader_program();
		wp.add_shader(a_loader->shader_lib["basic.glsl"].c_str(), "basic.glsl", GL_VERTEX_SHADER);
		wp.add_shader(a_loader->shader_lib["all_white.glsl"].c_str(), "frag.glsl", GL_FRAGMENT_SHADER);
		wp.link_shaders();
		//wp.use_shaders();
	vbo tb;
	auto tile_buffer = eng->prepare_for(mymap);
	
	/*tb.init();
	    tb.buffer_data(dt);
	    tb
	    	.add_pointer("position", 3, GL_FLOAT)
	    	.add_pointer("tex_coord", 2, GL_FLOAT)
    	.attach(tb);*/
    

FT_Library ft_library;
  FT_Face ft_face;
  FT_Error ft_error;

  if ((ft_error = FT_Init_FreeType (&ft_library)))
    abort();
  if ((ft_error = FT_New_Face (ft_library, "./Acme 5 Compressed Caps Outline Xtnd.ttf", 0, &ft_face)))
    abort();
  if ((ft_error = FT_Set_Char_Size (ft_face, FONT_SIZE*64, FONT_SIZE*64, 0, 0)))
    abort();

  /* Create hb-ft font. */
  hb_font_t *hb_font;
  hb_font = hb_ft_font_create (ft_face, NULL);

  /* Create hb-buffer and populate. */
  hb_buffer_t *hb_buffer;
  hb_buffer = hb_buffer_create ();
  hb_buffer_add_utf8 (hb_buffer, "téxt", -1, 0, -1);
  hb_buffer_guess_segment_properties (hb_buffer);

  /* Shape it! */
  hb_shape (hb_font, hb_buffer, NULL, 0);

  /* Get glyph information and positions out of the buffer. */
  unsigned int len = hb_buffer_get_length (hb_buffer);
  hb_glyph_info_t *info = hb_buffer_get_glyph_infos (hb_buffer, NULL);
  hb_glyph_position_t *pos = hb_buffer_get_glyph_positions (hb_buffer, NULL);

  /* Print them out as is. */
  printf ("Raw buffer contents:\n");
  for (unsigned int i = 0; i < len; i++)
  {
    hb_codepoint_t gid   = info[i].codepoint;
    unsigned int cluster = info[i].cluster;
    double x_advance = pos[i].x_advance / 64.;
    double y_advance = pos[i].y_advance / 64.;
    double x_offset  = pos[i].x_offset / 64.;
    double y_offset  = pos[i].y_offset / 64.;

    char glyphname[32];
    hb_font_get_glyph_name (hb_font, gid, glyphname, sizeof (glyphname));

    printf ("glyph='%s'	cluster=%d	advance=(%g,%g)	offset=(%g,%g)\n",
            glyphname, cluster, x_advance, y_advance, x_offset, y_offset);
  }

  printf ("Converted to absolute positions:\n");
  /* And converted to absolute positions. */
  {
    double current_x = 0;
    double current_y = 0;
    for (unsigned int i = 0; i < len; i++)
    {
      hb_codepoint_t gid   = info[i].codepoint;
      unsigned int cluster = info[i].cluster;
      double x_position = current_x + pos[i].x_offset / 64.;
      double y_position = current_y + pos[i].y_offset / 64.;


      char glyphname[32];
      hb_font_get_glyph_name (hb_font, gid, glyphname, sizeof (glyphname));

      printf ("glyph='%s'	cluster=%d	position=(%g,%g)\n",
	      glyphname, cluster, x_position, y_position);

      current_x += pos[i].x_advance / 64.;
      current_y += pos[i].y_advance / 64.;
    }
  }

  /* Draw, using cairo. */
  double width = 2 * MARGIN;
  double height = 2 * MARGIN;
  for (unsigned int i = 0; i < len; i++)
  {
    width  += pos[i].x_advance / 64.;
    height -= pos[i].y_advance / 64.;
  }
  if (HB_DIRECTION_IS_HORIZONTAL (hb_buffer_get_direction(hb_buffer)))
    height += FONT_SIZE;
  else
    width  += FONT_SIZE;

  /* Set up cairo surface. */
  cairo_surface_t *cairo_surface;
  cairo_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
					      ceil(width),
					      ceil(height));
  cairo_t *cr;
  cr = cairo_create (cairo_surface);
  // last one is alpha
  cairo_set_source_rgba (cr, 1., 1., 1., 0.);
  cairo_paint (cr);
  cairo_set_source_rgba (cr, 0., 0., 0., 1.);
  cairo_translate (cr, MARGIN, MARGIN);

  /* Set up cairo font face. */
  cairo_font_face_t *cairo_face;
  cairo_face = cairo_ft_font_face_create_for_ft_face (ft_face, 0);
  cairo_set_font_face (cr, cairo_face);
  cairo_set_font_size (cr, FONT_SIZE);

  /* Set up baseline. */
  if (HB_DIRECTION_IS_HORIZONTAL (hb_buffer_get_direction(hb_buffer)))
  {
    cairo_font_extents_t font_extents;
    cairo_font_extents (cr, &font_extents);
    double baseline = (FONT_SIZE - font_extents.height) * .5 + font_extents.ascent;
    cairo_translate (cr, 0, baseline);
  }
  else
  {
    cairo_translate (cr, FONT_SIZE * .5, 0);
  }

  cairo_glyph_t *cairo_glyphs = cairo_glyph_allocate (len);
  double current_x = 0;
  double current_y = 0;
  for (unsigned int i = 0; i < len; i++)
  {
    cairo_glyphs[i].index = info[i].codepoint;
    cairo_glyphs[i].x = current_x + pos[i].x_offset / 64.;
    cairo_glyphs[i].y = -(current_y + pos[i].y_offset / 64.);
    current_x += pos[i].x_advance / 64.;
    current_y += pos[i].y_advance / 64.;
  }
  cairo_show_glyphs (cr, cairo_glyphs, len);
  cairo_glyph_free (cairo_glyphs);
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
  eng_texture hb_tb(texture, tex_w, tex_h,tex_w, tex_h);
  texture_viewer.init();
      texture_viewer.buffer_data(gen::texview(hb_tb,9));
    texture_viewer
        .add_pointer("position", 3, GL_FLOAT)
        .add_pointer("tex_coord", 2, GL_FLOAT)
      .attach(texture_viewer);
	while (1) {
		glx::poll();
				/*dbgprint("u:%s d:%s l:%s r:%s \n"
			,keys[XK_Up]?"!":"-"
			,keys[XK_Down]?"!":"-"
			,keys[XK_Left]?"!":"-"
			,keys[XK_Right]?"!":"-");*/
		if (glx::done) {
			glx::clean_x();
			return 0;
		}		
		
		if (keys[XK_Up]) {
			//dbgprint("up!\n");
			pany += float(MAG);
		}
		if(keys[XK_Down]) {
			//dbgprint("dw!\n");
			pany -= float(MAG);
		}
		if(keys[XK_Left]) {
			//dbgprint("lf!\n");
			panx += float(MAG);
		}
		if(keys[XK_Right]) {
			//dbgprint("rt!\n");
			panx -= float(MAG);
		}
		pan_view(panx,pany);
		glx::clear_buffers();
		// draw tiles
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t.texture_id);
		sp.use_shaders();
		{
			auto projection_Location = sp.uniform("projection");
				glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
			auto model_loc = sp.uniform("model");
				glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(pan));
		}
		glBindVertexArray(tile_buffer.vao_id);
		glDrawArrays(GL_TRIANGLES, 0, tile_buffer.num_els);

		// draw text
		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f.texture.texture_id);
		//wp.use_shaders();
		{
			auto projection_Location = sp.uniform("projection");
				glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
			auto model_loc = sp.uniform("model");
				glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		}
		glBindVertexArray(tb.vao_id);
		glDrawArrays(GL_TRIANGLES, 0, tb.num_els);*/
		
		// texture viewer
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, hb_tb.texture_id);
		auto projection_Location = sp.uniform("projection");
				glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
		auto model_loc = sp.uniform("model");
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(glm::mat4()));
		glBindVertexArray(texture_viewer.vao_id);
		glDrawArrays(GL_TRIANGLES, 0, texture_viewer.num_els);

		//printf("%d\n", texture_viewer.num_els);
		glx::swap();
	}
	glx::clean_x();

	return 0;
}
