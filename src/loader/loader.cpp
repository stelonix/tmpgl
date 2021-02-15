#include <dirent.h>
#include <string_view>
#include <memory>
#include "loader/assets.h"
#include "engine/engine.h"
#include "loader/loader.h"
#include "helpers/helpers.h"
#include "helpers/util.h"
using namespace std;

void loader::add_file(string file, string loader_type, bool lazy = false) {
	project_files[file] = { loader_type, lazy };
}

string loader::find_dir(string dir) {
	dir = trim(dir, "/");

	while (dir_opts.find(dir) == dir_opts.end()) {
		dir = trim(util::ReplaceString(dir, util::basename_s(dir), ""), "/");
	}
	return dir;
}

folder_option loader::get_dir_opts(string dir) {
	return dir_opts[dir];
}

void loader::load(string file, string type) {
	printf("Loading %s as %s\n", file.c_str(), type.c_str());
	if (type == "FNT") {
		//a_loader.

	} else if (type == "LUA") {

	} else if (type == "MAP") {
		a_loader.load_map(file);
	} else if (type == "SHD") {
		a_loader.load_shader(file);
	} else if (type == "SPR") {
		a_loader.load_sprite(file);
	} else if (type == "TEX") {
		a_loader.load_as_texture(file);
	} else if (type == "TIL") {
		a_loader.load_tileset(file);
	} else {

	}
}
bool str_ends_with(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
void loader::enum_files(string cur_dir)
{
	struct dirent *dp;
	std::vector<string> dirs_to_enter;

	auto replaced = util::ReplaceString(cur_dir, project_path, "");

	auto dir = opendir(cur_dir.c_str());
	while ((dp = readdir(dir)) != NULL)
	{
		if ('.' != dp->d_name[0])
		{
			if (dp->d_type == DT_DIR)
			{
				dirs_to_enter.push_back(cur_dir + '/' + dp->d_name);
			}
			else if (cur_dir != project_path)
			{
				auto fileName = cur_dir + '/' + dp->d_name;
				folder_option opts = get_dir_opts(find_dir(replaced));
				for (auto ext : opts.exts) {
					if (str_ends_with(fileName, ext)) {
						add_file(fileName, opts.loader_type, opts.lazy);
						printf("[%s] %s/%s\n", opts.loader_type.c_str(), cur_dir.c_str(), dp->d_name);
					}
				}
			}
		}
	}
	closedir(dir);
	for (auto it = dirs_to_enter.begin(); it != dirs_to_enter.end(); it++)
	{
		enum_files((*it).c_str());
	}
}

void loader::add_dir(string dir, json data)
{
	folder_option cur_opt;
	for (auto it = data["exts"].begin(); it != data["exts"].end(); it++)
	{
		cur_opt.exts.push_back(*it);
	}
	cur_opt.loader_type = data["type"];
	if (data.count("lazy") > 0) cur_opt.lazy = data["lazy"];
	dir_opts[dir] = cur_opt;
}

/*			assets		*/

game_map loader::get_map(string file) {
	return a_loader.loaded_maps[resolve_file(file, "MAP")];
}

game_sprite loader::get_sprite(string file) {
	return a_loader.loaded_sprites[resolve_file(file, "SPR")];
}

game_sprite* loader::get_sprite_ptr(string file) {
    try {
       return &a_loader.loaded_sprites[resolve_file(file, "SPR")];
   } catch (...) {
       printf("exception thrown\n");
   }
   return NULL;
}

eng_texture loader::get_texture(string file) {
	return a_loader.loaded_tex[resolve_file(file, "TEX")];
}

eng_texture* loader::get_texture_ptr(string file) {
	return &a_loader.loaded_tex[resolve_file(file, "TEX")];
}

game_tileset loader::get_tileset(string file)
{
	return a_loader.loaded_tilesets[resolve_file(file, "TIL")];
}

game_tileset* loader::get_tileset_ptr(string file)
{
	return &a_loader.loaded_tilesets[resolve_file(file, "TIL")];
}

string loader::get_shader(string file)
{
	return a_loader.shader_lib[resolve_file(file, "SHD")];
}

/*			project		*/

void loader::load_project(string dirs_path)
{
	project_path = util::dirname_s(dirs_path);
	auto json = json::parse(read_file<string>(dirs_path));
	for (auto it = json.begin(); it != json.end(); it++)
	{
		process_dir("", it);
	}
	enum_files(project_path);
	for (auto it = dir_opts.begin(); it != dir_opts.end(); it++)
	{
		if (default_dirs.find(it->second.loader_type) == default_dirs.end())
		{
			default_dirs[it->second.loader_type] = it->first;
			dir_types[it->first] = it->second.loader_type;
			//printf("default for %s is %s\n", it->second.loader_type.c_str(), it->first.c_str());
		}
	}
	for (auto it = project_files.begin(); it != project_files.end(); it++)
	{
		if (!it->second.lazy) load(it->first, it->second.type);
	}
}

void loader::process_dir(string dir, json::iterator data)
{
	if (dir == "")
	{
		add_dir(data.key(), *data);
	}
	for (auto it = (*data).begin(); it != (*data).end(); it++)
	{
		if (it.key() == "exts" || it.key() == "type" || it.key() == "lazy") continue;
		add_dir(data.key()+it.key(), *it);
		process_dir(data.key()+it.key(), it);
	}
}

string loader::resolve_file(string file, string type)
{
	auto retval = project_path;
	auto def_dir = default_dirs[type];
	//printf("for file %s\n", file.c_str());
	if(util::basename_s(file) == file) {
		retval += "/" + def_dir;
	}
	//printf("with type = %s and default dir = %s\n", type.c_str(), def_dir.c_str());
	//printf("retval: %s\n", (retval + "/" + file).c_str());
	return retval + "/" + file;
}

string loader::resolve_type(string file)
{
	if (file[0] == '/') file = file.substr(1);
	return dir_types[file.substr(0, file.find_first_of('/'))];
}
