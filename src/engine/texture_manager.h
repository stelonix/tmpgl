#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include "engine/rt/eng_texture.h"

using namespace std;

struct texture_info {
	bool loaded;
	string path;

};

struct texture_manager {
	map<string, texture_info> available_textures;
	map<string, eng_texture> mapped_textures;
	eng_texture get_texture(string filename);
	eng_texture load_as_texture(string filename);
};

#endif
