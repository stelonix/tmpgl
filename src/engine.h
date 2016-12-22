#ifndef ENGINE_H
#define ENGINE_H

#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include "eng_sprite.h"
#include "game_map.h"
#include "game_sprite.h"
#include "string"
#include "shader.h"
#include "text_engine.h"
#include "vbo.h"
using shader_program = scene::shader_program;
struct eng_object {
	int x, y, z;
	int w, h;
	string name;
	std::function<void(eng_object*, int, int)> click_function;
	eng_object(string name);
	eng_object();
	void click_event(int x, int y);
};

struct rect {
	int x,y,w,h;
};

struct eng_animation {
	//std::vector<> frames
};

struct game_engine {
	// Data
	std::vector<eng_object> objects;
	std::vector<eng_sprite> sprites;
	vbo sprite_vbo;

	string root_dir;
	eng_object* selected;
	text_engine te;
	glm::mat4 projection;
	glm::mat4 view;
	int screen_w, screen_h;
	
	// Constructors
	game_engine(int w, int h);

	// Functions
	void add_sprite(game_sprite* spr, int x, int y, int layer);
	void build_sprites();
	void click_event(int x, int y);
	void draw_text(string text);

	void* get_asset(string name);
	font get_font(string name);
	
	void init();
	void load_project(string base_dir);
	shader_program make_shader(std::vector<string> files);
	vbo prepare_for(game_map mymap);
	void setup(int w, int h);
	void setup_linux();
	void setup_gui(int w, int h);
	coord_grid texture_viewer(eng_texture txt, string name, int x = 0, int y = 0, int z = 0);
	bool tick(int val);
	
};



#endif