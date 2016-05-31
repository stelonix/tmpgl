#ifndef ENGINE_H
#define ENGINE_H

#include <functional>
#include <vector>
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
	string root_dir;
	text_engine te;
	eng_object* selected;
	std::vector<eng_object> objects;

	game_engine();
	coord_grid texture_viewer(eng_texture txt, string name, int x = 0, int y = 0, int z = 0);
	void load_shaders();
	vbo prepare_for(game_map mymap);
	void draw_text(string text);
	void click_event(int x, int y);
};



#endif