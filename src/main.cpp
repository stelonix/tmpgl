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
#include "game_map.h"
#include "generators.h"
#include "game_sprite.h"
#include "helpers.h"
#include "shader.h"
#include "debug.h"
#include "engine.h"
#include <ft2build.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H 
#include "fonts.h"

using namespace cfg;
using json = nlohmann::json;
using shader_program = scene::shader_program;


std::map<int, int> keys;
float panx, pany;

void init_crt() {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
}

asset_loader* a_loader;
std::map<string, string>* shaders;

glm::mat4 pan;

void pan_view(float x, float y) {
	pan = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}
coord_grid vertex_data;
FT_Library  library;
int main(int argc, char *argv[]) {
	init_crt();
	
	bool error = FT_Init_FreeType( &library );
	

	
	font f;


	a_loader = new asset_loader();
	shaders = &a_loader->shader_lib;
	auto mymap = game_map::from_json(read_file<string>(ASSETS_DIR+"map.json"));
	a_loader->load_tileset(ASSETS_DIR+"tileset.json");
	game_sprite::from_json(read_file<string>(ASSETS_DIR+"sprite.json"));
	
	engine::load_shaders();
	glx::setup_x(HORZ_RES, VERT_RES);
	f.load();
	auto t = a_loader->load_texture(ASSETS_DIR+"indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	printf("Texture id: %d\nw: %d (%d)\nh: %d (%d)\n",
			t.texture_id,t.w, t.internal_w,
									t.h, t.internal_h);
	XEvent xev;
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
	
	auto vb = gen::vertex_grid(20, 15, 1);
    auto gt = mymap.flatten_layer(0);
    auto tc = gen::texture_map(gt, a_loader);
    
    vertex_data = gen::intercalate<3,2>(vb, tc);
    
    unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_data.size(), vertex_data.data(), GL_STATIC_DRAW);
	auto loc = sp.attrib("position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	loc = sp.attrib("tex_coord");
		glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(loc);

	while (1) {
		glx::poll();
				dbgprint("u:%s d:%s l:%s r:%s \n"
			,keys[XK_Up]?"!":"-"
			,keys[XK_Down]?"!":"-"
			,keys[XK_Left]?"!":"-"
			,keys[XK_Right]?"!":"-");
		if (glx::done) {
			glx::clean_x();
			return 0;
		}		
		
		if (keys[XK_Up]) {
			dbgprint("up!\n");
			pany += float(MAG);
		}
		if(keys[XK_Down]) {
			dbgprint("dw!\n");
			pany -= float(MAG);
		}
		if(keys[XK_Left]) {
			dbgprint("lf!\n");
			panx += float(MAG);
		}
		if(keys[XK_Right]) {
			dbgprint("rt!\n");
			panx -= float(MAG);
		}
		pan_view(panx,pany);
		glx::clear_buffers();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t.texture_id);
		sp.use_shaders();
		auto projection_Location = sp.uniform("projection");
			glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
		auto model_loc = sp.uniform("model");
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(pan));

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 9000);
		glx::swap();
	}
	glx::clean_x();

	return 0;
}
