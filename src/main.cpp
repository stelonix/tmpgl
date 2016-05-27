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
#include "text_engine.h"
#include "vbo.h"
#include <hb.h>
#include <hb-ft.h>
#include <cairo.h>
#include <cairo-ft.h>
#include "logging.h"

#define FONT "./Sevastopol-Interface.ttf"
#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)
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

int main(int argc, char *argv[]) {
	init_crt();
	setup_stdout();
	setup_threads();
	
	
	eng = new game_engine();
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
	glm::mat4 view = glm::lookAt(
				glm::vec3(0,0,1), // Camera is at (0,0,5), in World Space
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	auto VP = projection*view;
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
    

	text_engine te;
	te.init();
	te.load_font(FONT, FONT_SIZE);
	auto txt = te.draw_text(FONT, "Teste magnífico de encapsulamento de text_rendering né? ÇAÇABA!");

	//eng_texture hb_tb(texture, tex_w, tex_h,tex_w, tex_h);
  texture_viewer.init();
      texture_viewer.buffer_data(gen::texview(txt,9));
    texture_viewer
        .add_pointer("position", 3, GL_FLOAT)
        .add_pointer("tex_coord", 2, GL_FLOAT)
      .attach(texture_viewer);
	while (1) {
		glx::poll();
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

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, txt.texture_id);
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
