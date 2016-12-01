#include <libgen.h>
#include "assets.h"
#include "cfg.h"
#include "helpers.h"
#include "util.h"

extern GLuint png_texture_load(const char* file_name, int* width, int* height,
								int* real_w=NULL, int* real_h=NULL);

GLuint texture(string filename) {
	int ww, hh;
	auto retval = png_texture_load(filename.c_str(),&ww,&hh);
	return retval;
}

eng_texture asset_loader::load_as_texture(string filename) {
	int w, h, rw, rh;
	auto tx = png_texture_load(filename.c_str(),&w,&h, &rw, &rh);
	auto retval = eng_texture(tx, w, h, rw, rh);
	loaded_tex[filename] = retval;
	printf("Texture id: %d\nw: %d (%d)\nh: %d (%d)\n",
			retval.texture_id,retval.w, retval.internal_w,
									retval.h, retval.internal_h);
	
	return retval;
}

string asset_loader::load_shader(string filename) {
	auto file_c = read_file<string>(filename);
	shader_lib[filename] = file_c;
	return file_c;
}

game_map asset_loader::load_map(string filename) {
	loaded_maps[filename] = game_map::from_json(
		read_file<string>(filename));
	return loaded_maps[filename];
}

game_sprite asset_loader::load_sprite(string filename) {
	loaded_sprites[filename] = game_sprite::from_json(
		read_file<string>(filename));
	return loaded_sprites[filename];
}

game_tileset asset_loader::load_tileset(string filename) {
	loaded_tilesets[filename] = game_tileset::from_json(
		read_file<string>(filename));
	return loaded_tilesets[filename];
}

void  asset_loader::require(string filepath)
{
	printf("%s\n", util::dirname_s(filepath).c_str());
}

