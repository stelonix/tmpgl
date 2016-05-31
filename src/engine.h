#ifndef ENGINE_H
#define ENGINE_H

#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include "game_map.h"
#include "string"
#include "text_engine.h"
#include "vbo.h"

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
	string root_dir;
	eng_object* selected;
	text_engine te;
	glm::mat4 projection;
	glm::mat4 view;
	int screen_w, screen_h;
	
	// Constructors
	game_engine(int w, int h);

	// Functions
	void click_event(int x, int y);
	void draw_text(string text);
	void init();
	void load_shaders();
	void load_project(string base_dir);
	vbo prepare_for(game_map mymap);
	void setup(int w, int h);
	void setup_linux();
	void setup_gui(int w, int h);
	coord_grid texture_viewer(eng_texture txt, string name, int x = 0, int y = 0, int z = 0);

	
};



#endif