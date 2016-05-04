#include <string>
#include <vector>
#include <GL/gl.h>
#include <tuple>
#include "eng_texture.h"

using namespace engine;
extern GLuint png_texture_load(const char* file_name, int* width, int* height);

GLuint texture(std::string filename);

struct asset_loader {
	std::vector<eng_texture> loaded_tex;

	eng_texture load_texture(std::string filename);
};

