#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include "string"
#include "vbo.h"

struct eng_object {
	int x, y, z;

};

struct game_engine {
	string root_dir;
	game_engine();
	eng_object* selected;
	std::vector<eng_object> objs;
	void load_shaders();
	vbo prepare_for(game_map mymap);
};



#endif