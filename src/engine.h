#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "string"
#include "text_engine.h"
#include "vbo.h"

struct eng_object {
	int x, y, z;

};

struct game_engine {
	string root_dir;
	text_engine te;
	eng_object* selected;
	std::vector<eng_object> objs;

	game_engine();
	void load_shaders();
	vbo prepare_for(game_map mymap);
	void draw_text(string text);
};



#endif