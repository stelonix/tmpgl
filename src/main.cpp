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
#include "assets.h"
#include "cfg.h"
#include "boilerplate.h"
#include "game_map.h"
#include "game_sprite.h"
#include "helpers.h"
#include "shader.h"
#include "unwind.h"

using namespace cfg;
using json = nlohmann::json;
using shader_program = scene::shader_program;


std::map<int, int> keys;
float panx, pany;
/*namespace scene {
	shader_program make_shader(std::array<const char*> shd) {
		auto sp = shader_program();
		for (auto it = shd.begin(); it != shd.end(); it++)
		{
			sp.add_shader(a_loader->shader_lib[*it].c_str(), *it, GL_VERTEX_SHADER);
		}
		
		sp.add_shader(a_loader->shader_lib["frag.glsl"].c_str(), "frag.glsl", GL_FRAGMENT_SHADER);
		sp.link_shaders();
	}
}*/
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

int main(int argc, char *argv[]) {
	init_crt();


	a_loader = new asset_loader();
	shaders = &a_loader->shader_lib;
	game_map::from_json(read_file(ASSETS_DIR+"map.json"));
	tileset::from_json(read_file(ASSETS_DIR+"tileset.json"));
	game_sprite::from_json(read_file(ASSETS_DIR+"sprite.json"));
	
	for (std::string s : list_files(SHADER_DIR)) {
		a_loader->load_shader(SHADER_DIR+"/"+s);
		//printf("%s\n---\n%s\n---\n", s.c_str(), shaders[s].c_str());
	}
	glx::setup_x();
	auto t = a_loader->load_texture(ASSETS_DIR+"indoor_free_tileset__by_thegreatblaid-d5x95zt.png");
	printf("Texture id: %d\nw: %d (%d)\nh: %d (%d)",
			t.texture_id,t.w, t.internal_w,
									t.h, t.internal_h);
	XEvent xev;
	glx::init_glew();
	glx::init_gl(800, 600);
	auto projection = glm::ortho( 0.f, 800.f, 600.f, 0.0f, 0.0f, 100.f ); 
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
	
	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	int x = 0;
	int y = 0;
	int tx = 0;
	int ty = 1;
	
	float vertices[] ={
		x*TILE_SIZE,		y*TILE_SIZE,		0.0f,
			t.normalize_u(tx*ATILE), t.normalize_v(ty*ATILE),
		(x+1)*TILE_SIZE,	y*TILE_SIZE,		0.0f,
			t.normalize_u((tx+1)*ATILE), t.normalize_v(ty*ATILE),
		(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	0.0f,
			t.normalize_u((tx+1)*ATILE), t.normalize_v((ty+1)*ATILE),
		x*TILE_SIZE,		y*TILE_SIZE,	0.0f,
			t.normalize_u(tx*ATILE), t.normalize_v(ty*ATILE),
		x*TILE_SIZE,	(y+1)*TILE_SIZE,	0.0f,
			t.normalize_u(tx*ATILE), t.normalize_v((ty+1)*ATILE),
		(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,		0.0f,
			t.normalize_u((tx+1)*ATILE), t.normalize_v((ty+1)*ATILE)
	};
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	auto loc = sp.attrib("position");
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	loc = sp.attrib("tex_coord");
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(loc);
    auto vb = gen_map_sh(20,15,1);
    printf("%d\n", vb.size());
    int count = 0;
    for (auto i = vb.begin(); i != vb.end(); ++i)
    {
    	printf("%d: %f\n",count, *i);
    	count++;
    	if (count >= 30) break;
    }
    //exit(0);
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.3, 0.3, 0.0f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t.texture_id);
		sp.use_shaders();
		
		

		auto projection_Location = sp.uniform("projection");
		glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
			//auto texLoc = scene::get_uniform_loc("tex");
			//glUniform1i(texLoc, texture);
			//glActiveTexture(GL_TEXTURE0);
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
		
		auto model_loc = sp.uniform("model");
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(pan));

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glx::swap();
	}
	glx::clean_x();

	return 0;
}
