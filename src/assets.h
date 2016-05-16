#ifndef ASSETS_H
#define ASSETS_H

#include <map>
#include <tuple>
#include <vector>
#include <GL/gl.h>
#include "aliases.hpp"
#include "string"
#include "eng_texture.h"
#include "game_map.h"
#include "game_sprite.h"
#include "game_tileset.h"
#include "helpers.h"

using namespace engine;
using namespace std;

GLuint texture(string filename);

struct asset_loader {
	path_map<string> shader_lib;
	path_map<eng_texture> loaded_tex;
	path_map<game_map> loaded_maps;
	path_map<game_sprite> loaded_sprites;
	path_map<game_tileset> loaded_tilesets;

	eng_texture load_texture(string filename);
	string load_shader(string filename);
	game_map load_map(string filename);
	game_sprite load_sprite(string filename);
	game_tileset load_tileset(string filename);
	int sweep_load(string directory);

	

};
/*template <typename T, LoadFunc, typename AsType = string>
T load_simple(
	string filename,
	path_map<string>& path_store)
{
	path_store[filename] = LoadFunc(read_file<AsType>(filename));
	return path_store[filename];
}*/
#endif