#include <libgen.h>
#include "assets.h"
#include "helpers.h"

extern GLuint png_texture_load(const char* file_name, int* width, int* height,
								int* real_w=NULL, int* real_h=NULL);


GLuint texture(string filename) {
	int ww, hh;
	auto retval = png_texture_load(filename.c_str(),&ww,&hh);
	return retval;
}

eng_texture asset_loader::load_texture(string filename) {
	int w, h, rw, rh;
	auto tx = png_texture_load(filename.c_str(),&w,&h, &rw, &rh);
	auto retval = eng_texture(tx, w, h, rw, rh);
	loaded_tex[filename] = retval;
	return retval;
}

string asset_loader::load_shader(string filename) {
	auto ptr = const_cast<char*>(filename.c_str());
	auto file_c = read_file(filename);
	shader_lib[basename(ptr)] = file_c;
	return file_c;
}

game_map asset_loader::load_map(string filename) {
	loaded_maps[filename] = game_map::from_json(
		read_file(filename));
	return loaded_maps[filename];
}

game_sprite asset_loader::load_sprite(string filename) {
	loaded_sprites[filename] = game_sprite::from_json(
		read_file(filename));
	return loaded_sprites[filename];
}

game_tileset asset_loader::load_tileset(string filename) {
	printf("Loading tileset: %s\n", filename.c_str());
	loaded_tilesets[filename] = game_tileset::from_json(
		read_file(filename));
	return loaded_tilesets[filename];
}