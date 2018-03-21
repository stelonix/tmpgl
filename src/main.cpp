#define _XOPEN_SOURCE 500			/* Required under GLIBC for nftw() */
//#define _XOPEN_SOURCE_EXTENDED 1	/* Same */
#include "helpers/string"
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <dirent.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include "include/json.hpp"
#include "include/alphanum.hpp"
#include "include/prettyprint.hpp"
#include "loader/assets.h"
#include "cfg.h"
#include "boilerplate.h"
#include "debug.h"
#include "engine/engine.h"
#include "engine/text/fonts.h"
#include "engine/datatypes/game_map.h"
#include "helpers/generators.h"
#include "engine/datatypes/game_sprite.h"
#include "helpers/helpers.h"
#include "engine/shader.h"
#include "engine/text/text_engine.h"
#include "engine/vbo.h"
#include <hb.h>
#include <hb-ft.h>
#include <cairo.h>
#include <cairo-ft.h>
#include "logging.h"
#include <libgen.h>
#include <ftw.h>
#include <dirent.h>
#include "helpers/util.h"
#include "include/imgui/imgui.h"
#include "loader/loader.h"
#include "engine/atlas/texture_atlas.h"

#define FONT "./Sevastopol-Interface.ttf"
#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)
using namespace cfg;
using json = nlohmann::json;
using shader_program = scene::shader_program;

shader_program* current_program;
std::map<int, int> keys;

glm::mat4 pan;
float panx, pany;

void pan_view(float x, float y)
{
	pan = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}
loader p_loader;
vbo texture_viewer;
game_engine* eng;

timespec diff(timespec start, timespec end)
{
	timespec temp;
	temp.tv_sec = end.tv_sec-start.tv_sec;
	temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	if ((end.tv_nsec - start.tv_nsec) < 0)
	{
			temp.tv_sec -= 1;
			temp.tv_nsec += 1000000000;
	}
	return temp;
}
int tick_count = 0;
int main(int argc, char *argv[])
{
	panx = 0.0f;pany = 0.0f;
	eng = new game_engine(HORZ_RES, VERT_RES, &p_loader);
	p_loader.load_project("./sample_project/dirs.json");
	auto mymap = p_loader.get_map("maps/map.json");
	auto t = p_loader.get_texture_ptr("tiles/img/indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	t->build_cache(cfg::ATILE);
	auto VP = eng->projection * eng->view;
	auto sp = eng->make_shader({"basic.vertex", "frag.glsl"});
		sp.use_shaders();
	auto wp = eng->make_shader({"basic.vertex", "all_white.glsl"});
	auto tile_buffer = eng->prepare_for(mymap);

	text_engine te;
	te.init();
	te.load_font(FONT, FONT_SIZE);
	auto txt = te.draw_text(FONT, "Teste magnífico de encapsulamento de text_rendering né? ÇAÇABA!");

	texture_viewer.init();
		texture_viewer.buffer(eng->texture_viewer(txt,"texview1", 300,200,9));
		texture_viewer
			.add_pointer("position", 3, GL_FLOAT)
			.add_pointer("tex_coord", 2, GL_FLOAT)
	.attach(texture_viewer);

	eng->objects["texview1"].click_function = [](eng_object* t, int x, int y)
	{
		eng->selected = t;
		//printf("%s\n", t->name.c_str());
	};
	texture_atlas atlas;

	eng->make_atlas({"sprites/sprite.json"});
	//dbgprint("asdf %d", 1);
	//glx::done = true;
	auto tmps = p_loader.get_sprite_ptr("sprite.json");
	//atlas.add(tmps->states["walking"][0]);
	//atlas.add(tmps->states["walking"][1]);
	auto tframe = p_loader.get_tileset_ptr("tileset.json")->tiles[0][0];
	//atlas.add(tframe.img, tframe.u, tframe.v, cfg::ATILE, cfg::ATILE);
	//atlas.pack(1024, 1024);
	auto mat_move = glm::vec3(300,200,0);
	long frames = 0;
	auto s = p_loader.get_texture_ptr("sprites/img/sprite_sheet___aege_by_destructionseries-d5dg2g2.png");
	s->build_cache(p_loader.get_sprite_ptr("sprite.json")->states["walking"]);
	//eng->add_sprite(p_loader.get_sprite_ptr("sprite.json"), 1, 0, 9);
	//eng->add_sprite(p_loader.get_sprite_ptr("sprite.json"), 10, 20, 9);
	while (1)
	{
		struct timespec start, end;
		glx::poll();
		if (glx::done) {
			glx::clean_x();
			return 0;
		}
		clock_gettime(CLOCK_MONOTONIC, &start);

		if (keys[XK_Up])	pany += float(MAG);
		if (keys[XK_Down])	pany -= float(MAG);
		if (keys[XK_Left])	panx += float(MAG);
		if (keys[XK_Right])	panx -= float(MAG);
		if (keys[XK_Escape]) break;

		pan_view(panx,pany);
		glx::clear_buffers();
		// draw tiles
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->texture_id);
		sp.use_shaders();
		{
			sp.uniform("projection", VP);
			sp.uniform("model", pan);
		}
		sp.draw(tile_buffer);
		glBindTexture(GL_TEXTURE_2D, s->texture_id);
		sp.draw(eng->sprite_vbo);
		mat_move = glm::vec3(eng->objects["texview1"].x, eng->objects["texview1"].y,0);
		glBindTexture(GL_TEXTURE_2D, txt.texture_id);
			sp.uniform("model", glm::mat4());
			sp.uniform("v_trans", mat_move);
		sp.draw(texture_viewer);

		if (eng->selected != NULL)
		{
			wp.use_shaders();
			wp.uniform("projection", VP);
			wp.uniform("model", glm::mat4());
			wp.uniform("v_trans", mat_move);
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			wp.draw(texture_viewer);
		}
		sp.use_shaders();
		sp.uniform("v_trans", glm::vec3(0.,0.,0.));
		glx::swap();

		clock_gettime(CLOCK_MONOTONIC, &end);
		auto msecs = int(diff(start, end).tv_nsec/1.0e6);
		float fps;
		frames++;
		if (eng->tick(msecs)) eng->build_sprites();
		tick_count += msecs;
		if (tick_count >= 1000)
		{
			fps = float(frames) / float(msecs)*1000;
			frames = 0;
			char buf[200];
			sprintf(buf, "gl3 %.1f", fps);
			tick_count = 0;
			glx::set_title(buf);
		}
	}
	glx::clean_x();
	return 0;
}
