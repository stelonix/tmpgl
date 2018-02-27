#include "engine/texture_manager.h"

extern GLuint png_texture_load(const char* file_name, int* width, int* height,
								int* real_w=NULL, int* real_h=NULL);

eng_texture texture_manager::get_texture(string filename) {
	if (!available_textures[filename].loaded) load_as_texture(filename);
	return mapped_textures[filename];
}

eng_texture texture_manager::load_as_texture(string filename) {
	int w, h, rw, rh;
	auto tx = png_texture_load(filename.c_str(),&w,&h, &rw, &rh);
	auto retval = eng_texture(tx, w, h, rw, rh);
	mapped_textures[filename] =  retval;
	printf("Texture id: %d\nw: %d (%d)\nh: %d (%d)\n",
			retval.texture_id,retval.w, retval.internal_w,
									retval.h, retval.internal_h);

	return retval;
}
