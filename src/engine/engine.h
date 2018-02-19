#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <glm/glm.hpp>
#include "engine/rt/eng_sprite.h"
#include "engine/rt/eng_object.h"
#include "engine/datatypes/game_map.h"
#include "engine/datatypes/game_sprite.h"
#include "loader/loader.h"
#include "helpers/string"
#include "engine/shader.h"
#include "engine/atlas/atlas_builder.h"
#include "engine/text/text_engine.h"
#include "engine/vbo.h"
using shader_program = scene::shader_program;
using namespace std;


struct game_engine {
	// Data
	map<int, vector<vbo>> render_list;
	struct templs {
		using action_map = map<string, vector<quad_coords>>;
		using tile_map = map<int, vector<quad_coords>>;
		path_map<action_map> sprites;
		path_map<tile_map> tiles;
	} templates;

	vector<eng_object> objects;
	vector<eng_sprite> sprites;
	map<int, eng_texture> textures;
	map<string, GLint> mapped_textures;
	loader* game_loader;
	vbo sprite_vbo;

	string root_dir;
	eng_object* selected;
	text_engine te;
	glm::mat4 projection;
	glm::mat4 view;
	int screen_w, screen_h;

	// Constructors
	game_engine(int w, int h, loader* p_loader);

	// Game logic
	void add_sprite(game_sprite* spr, int x, int y, int layer);
	void build_sprites();
	void click_event(int x, int y);
	void draw_text(string text);

	// Assets
	void* get_asset(string name);
	font get_font(string name);

	// Setup
	void init();
	void load_project(string base_dir);

	// Resources
	eng_texture blank_texture(int w, int h);
	std::vector<eng_texture> make_atlas(std::vector<string> paths);
	shader_program make_shader(std::vector<string> files);
	vbo prepare_for(game_map mymap);
	void setup(int w, int h);
	void setup_linux();
	void setup_gui(int w, int h);
	coord_grid texture_viewer(eng_texture txt, string name, int x = 0, int y = 0, int z = 0);
	bool tick(int val);

};



#endif
