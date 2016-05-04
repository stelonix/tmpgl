#include "assets.h"

extern GLuint png_texture_load(const char* file_name, int* width, int* height);


GLuint texture(std::string filename) {
	int ww, hh;
	auto retval = png_texture_load(filename.c_str(),&ww,&hh);
	return retval;
}

eng_texture asset_loader::load_texture(std::string filename) {
		int w, h;
		auto tx = png_texture_load(filename.c_str(),&w,&h);
		auto retval = eng_texture(tx, w, h);
		loaded_tex.push_back(retval);
		return retval;
	}