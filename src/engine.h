#ifndef ENGINE_H
#define ENGINE_H

#include "string"
#include "vbo.h"

namespace engine {
	void load_shaders();
	vbo prepare_for(game_map mymap);
}

#endif