#ifndef ASSETS_H
#define ASSETS_H

#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <vector>
#include <GL/gl3w.h>
#include <GL/gl.h>
#include "aliases.hpp"
#include "helpers/string"
#include "engine/texture_manager.h"
#include "engine/rt/eng_texture.h"
#include "engine/datatypes/game_map.h"
#include "engine/datatypes/game_sprite.h"
#include "engine/datatypes/game_tileset.h"
#include "helpers/helpers.h"

using namespace engine;
using namespace std;

GLuint texture(string filename);

struct loaded_asset;
using ase_load_fn =
	std::function<loaded_asset(string)>;

struct asset_loader {
	// Pointer so we can have the same approach
	struct loaded_asset {
		void* data;
		int size;
		//ase_type type;
	};
	shared_ptr<texture_manager> tex_man;

	path_map<loaded_asset> assets;
	path_map<string> shader_lib;
	path_map<eng_texture> loaded_tex;
	path_map<game_map> loaded_maps;
	path_map<game_sprite> loaded_sprites;
	path_map<game_tileset> loaded_tilesets;

	//asset_loader();

	eng_texture load_as_texture(string filename);

	string load_shader(string filename);
	game_map load_map(string filename);
	game_sprite load_sprite(string filename);
	game_tileset load_tileset(string filename);

	void load_hook();

	int sweep_load(string directory);
	void require(string filepath);


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
