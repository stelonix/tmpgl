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



string dirname_s(char *path)
{
	auto ptr = strdup(path);
		auto retval = string(dirname(ptr));
	free(ptr);
	return retval;
}

string dirname_s(string path)
{
	return dirname_s(path.c_str());
}

vbo texture_viewer;
game_engine* eng;

int fn(const char* path, const struct stat* st, const int type, struct FTW* path_info)
{
	printf("%s\n", path);
}

int main(int argc, char *argv[]) {
	panx = 0.0f;pany = 0.0f;
	eng = new game_engine(HORZ_RES, VERT_RES);
	//ftw("sample_project/", fn, USE_FDS, FTW_PHYS);
	//printf("In %s/\n", dirname("sample_project/"));
	a_loader = new asset_loader();
	auto mymap = game_map::from_json(read_file<string>(ASSETS_DIR+"map.json"));
	a_loader->load_tileset(ASSETS_DIR+"tileset.json");
	game_sprite::from_json(read_file<string>(ASSETS_DIR+"sprite.json"));
	
	eng->load_shaders();
	
	auto t = a_loader->load_texture(ASSETS_DIR+"indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	
	auto VP = eng->projection * eng->view;
	auto sp = shader_program();
		sp.add_shader(a_loader->shader_lib["basic.glsl"].c_str(), "basic.glsl", GL_VERTEX_SHADER);
		sp.add_shader(a_loader->shader_lib["frag.glsl"].c_str(), "frag.glsl", GL_FRAGMENT_SHADER);
		sp.link_shaders();
		sp.use_shaders();
	auto wp = shader_program();
		wp.add_shader(a_loader->shader_lib["basic.glsl"].c_str(), "basic.glsl", GL_VERTEX_SHADER);
		wp.add_shader(a_loader->shader_lib["all_white.glsl"].c_str(), "all_white.glsl", GL_FRAGMENT_SHADER);
		wp.link_shaders();
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
