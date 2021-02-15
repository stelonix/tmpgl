#include <set>
#include <libgen.h>
#include <nlohmann/json.hpp>
#include "loader/assets.h"
#include "cfg.h"
#include "helpers/helpers.h"
#include "helpers/util.h"

// asset_loader::asset_loader(texture_manager* tex_man) {
// 	this->tex_man = tex_man;
// }

eng_texture asset_loader::load_as_texture(string filename) {
	auto retval = tex_man->load_as_texture(filename);
	loaded_tex[filename] = retval;
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
#define SET_INSERT(x, y) x.insert(y).second

nlohmann::json load_json(string fileName) {
	auto jsonString = read_file<string>(fileName);
	return nlohmann::json::parse(jsonString);
}

game_sprite asset_loader::load_sprite(string filename) {
	auto currentSprite = game_sprite::from_json(load_json(filename), filename);
	loaded_sprites[filename] = currentSprite;
	//@TODO: reimplement using jsoncons. replace nlohmann's
	set<string> foundFiles;

	for (auto states = currentSprite.states.begin(); states != currentSprite.states.end(); states++)
	{
		for (auto frame = states->second.begin(); frame != states->second.end(); frame++)
		{
			auto fullPath = "frame.img"_s;
			if (SET_INSERT(foundFiles, fullPath))
				printf("Dependency: %s\n", fullPath.c_str());
		}
	}
	
	//printf(spriteJson["/foo/baz"_json_pointer].c_str());
	return loaded_sprites[filename];
}

game_tileset asset_loader::load_tileset(string filename) {
	loaded_tilesets[filename] = game_tileset::from_json(
		read_file<string>(filename), filename);
	return loaded_tilesets[filename];
}

void  asset_loader::require(string filepath)
{
	printf("%s\n", util::dirname_s(filepath).c_str());
}
