#include <string>
#include <vector>
#include <GL/gl.h>
#include <tuple>
#include "eng_texture.h"

using namespace engine;
extern GLuint png_texture_load(const char* file_name, int* width, int* height);
namespace assets {
	GLuint texture(std::string filename);

	struct eng_loader {
		std::vector<eng_texture> loaded_tex;

		eng_texture& load_texture(std::string filename) {
			int w, h;
			auto tx = png_texture_load(filename.c_str(),&w,&h);
			auto retval = eng_texture(tx, w, h);
			loaded_tex.push_back(retval);
			return retval;
		}
	};
}

