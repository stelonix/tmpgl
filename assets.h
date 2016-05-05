#include <string>
#include <vector>
#include <GL/gl.h>
#include <tuple>
#include "eng_texture.h"

using namespace engine;

GLuint texture(std::string filename);

struct asset_loader {
	std::vector<eng_texture> loaded_tex;
	std::vector<std::string> shader_lib;

	eng_texture load_texture(std::string filename);
};

