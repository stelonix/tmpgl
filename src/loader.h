#ifndef LOADER_H
#define LOADER_H

#include "include/json.hpp"
#include "aliases.hpp"
#include "assets.h"
#include "string"
#include <map>
#include <vector>
using json = nlohmann::json;
struct fld_opts {
	std::vector<string> exts;
	bool lazy;
	string loader_type;
};
struct project_asset {
	string type;
	bool lazy;
};
struct loader {
	asset_loader a_loader;
	string project_path;
	std::map<string, fld_opts> dir_opts;
	std::map<string, project_asset> project_files;
	std::map<string, string> default_dirs;
	std::map<string, string> dir_types;

	void add_file(string file, string loader_type, bool lazy);
	string find_dir(string dir);
	fld_opts get_dir_opts(string dir);
	void enum_files(string cur_dir);

	game_sprite get_sprite(string name);
	game_sprite* get_sprite_ptr(string name);
	game_map get_map(string name);
	string get_shader(string name);
	eng_texture get_texture(string name);
	eng_texture* get_texture_ptr(string name);
	game_tileset get_tileset(string name);
	game_tileset* get_tileset_ptr(string name);

	void load(string file, string type);
	void load_project(string dirs_path);
	void add_dir(string dir, json data);
	void process_dir(string dir, json::iterator data);
	string resolve_file(string file, string type);
	string resolve_type(string file);
	//void load_game
};

template <typename GameType, typename EngType> struct res_info {
	GameType* game_obj;
	EngType* eng_obj;
};

#endif