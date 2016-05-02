#include "assets.h"

extern GLuint png_texture_load(const char* file_name, int* width, int* height);

namespace assets {
	GLuint texture(std::string filename) {
		int ww, hh;
		auto retval = png_texture_load(filename.c_str(),&ww,&hh);
		return retval;
	}
}