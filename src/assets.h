#include <map>
#include <tuple>
#include <vector>
#include <GL/gl.h>
#include "string"
#include "eng_texture.h"

using namespace engine;

GLuint texture(string filename);

struct asset_loader {
	std::vector<eng_texture> loaded_tex;
	std::map<string, string> shader_lib;

	eng_texture load_texture(string filename);
	std::string load_shader(string filename);
	int sweep_load(string directory);
};

