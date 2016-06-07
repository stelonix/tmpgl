#define _XOPEN_SOURCE 500			/* Required under GLIBC for nftw() */
//#define _XOPEN_SOURCE_EXTENDED 1	/* Same */
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
#include <time.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include "include/json.hpp"
#include "include/alphanum.hpp"
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
#include <libgen.h>
#include <ftw.h>
#include <dirent.h>
#include "util.h"
#include "include/imgui/imgui.h"

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

void pan_view(float x, float y) {
	pan = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}

vbo texture_viewer;
game_engine* eng;
typedef std::map<std::string, std::vector<string>, doj::alphanum_less<string> > m_t;
m_t files_to_load;
int fn(	const char* path,
		const struct stat* st,
		const int type,
		struct FTW* path_info)
{
	auto cd = util::dirname_s(path);
	auto entry_bname = util::basename_s(path);
	switch (type) {
		case FTW_F:
		{
			files_to_load[cd].push_back(entry_bname);
			break;
		}
		case FTW_D:
		{
			if (path_info->level > 1) break;
			if (files_to_load.find(entry_bname) == files_to_load.end())
				files_to_load[entry_bname] = std::vector<string>();
			break;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	panx = 0.0f;pany = 0.0f;
	eng = new game_engine(HORZ_RES, VERT_RES);
	nftw("./sample_project", fn, 20, FTW_PHYS);
	
	for (auto it = files_to_load.begin(); it != files_to_load.end(); it++)
	{
		std::sort(it->second.begin(), it->second.end(), doj::alphanum_less<string>());
	}
	for (auto it = files_to_load.begin(); it != files_to_load.end(); it++)
	{
		printf("dir %s has\n", it->first.c_str());
		for (int i = 0; i < it->second.size(); i++)
			printf("\t%s\n", it->second[i].c_str());
	}
	printf("map size %d\n", files_to_load.size());
	//exit(0);
	//printf("In %s/\n", dirname("sample_project/"));
	a_loader = new asset_loader();
	auto mymap = game_map::from_json(read_file<string>(ASSETS_DIR+"map.json"));
	a_loader->load_tileset(ASSETS_DIR+"tileset.json");
	game_sprite::from_json(read_file<string>(ASSETS_DIR+"sprite.json"));
	
	eng->load_shaders();
	
	auto t = a_loader->load_texture(ASSETS_DIR+"indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	
	auto VP = eng->projection * eng->view;
	auto sp = eng->make_shader({"basic.vertex", "frag.glsl"});
		sp.use_shaders();
	auto wp = eng->make_shader({"basic.vertex", "all_white.glsl"});
	auto tile_buffer = eng->prepare_for(mymap);
    

	text_engine te;
	te.init();
	te.load_font(FONT, FONT_SIZE);
	auto txt = te.draw_text(FONT, "Teste magnífico de encapsulamento de text_rendering né? ÇAÇABA!");

	//eng_texture hb_tb(texture, tex_w, tex_h,tex_w, tex_h);
	texture_viewer.init();
		texture_viewer.buffer(eng->texture_viewer(txt,"texview1", 300,200,9));
		texture_viewer
			.add_pointer("position", 3, GL_FLOAT)
			.add_pointer("tex_coord", 2, GL_FLOAT)
	.attach(texture_viewer);
	
	eng->objects[0].click_function = [](eng_object* t, int x, int y)
	{
		eng->selected = t;
		printf("%s\n", t->name.c_str());
	};
	
	auto mat_move = glm::vec3(300,200,0);
	time_t start, end;
	long frames = 0;
	while (1) {
		glx::poll();
		if (glx::done) {
			glx::clean_x();
			return 0;
		}
		start = time(NULL);
		if (keys[XK_Up])	pany += float(MAG);
		if (keys[XK_Down])	pany -= float(MAG);
		if (keys[XK_Left])	panx += float(MAG);
		if (keys[XK_Right])	panx -= float(MAG);
		if (keys[XK_Escape]) break;

		pan_view(panx,pany);
		glx::clear_buffers();
		// draw tiles
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		//glDepthFunc( GL_LEQUAL );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t.texture_id);
		sp.use_shaders();
		{
			sp.uniform("projection", VP);
			sp.uniform("model", pan);
		}
		sp.draw(tile_buffer);

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

		end = time(NULL);
		float fps;
		frames++;
		
		if (end - start > 0.25 )
		{
		    fps = float(frames) / float(end - start);
		    //printf("%d / %d-%d (%d) = %f\n", frames, end,start,end-start, fps);
		    start = end;
		    frames = 0;
		}
		char buf[200];
		sprintf(buf, "gl3 %.1f", fps);
		glx::set_title(buf);
	}
	glx::clean_x();
	return 0;
}
