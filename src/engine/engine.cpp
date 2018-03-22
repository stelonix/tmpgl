#include <glm/gtc/matrix_transform.hpp>
#include "loader/assets.h"
#include "boilerplate.h"
#include "data_transform.h"
#include "debug.h"
#include "engine/engine.h"
#include "engine/atlas/atlas_builder.h"
#include "helpers/generators.h"
#include "helpers/helpers.h"
#include "logging.h"
#include "helpers/util.h"
#include "cfg.h"
using namespace cfg;

#define FONT "./Sevastopol-Interface.ttf"
#define FONT_SIZE 36

game_engine::game_engine(int w, int h, loader* p_loader)
{
	screen_w = w; screen_h = h;
	tex_man = shared_ptr<texture_manager>(new texture_manager());
	p_loader->a_loader.tex_man = tex_man;
	game_loader = p_loader;
	setup(w, h);
	init();
	selected = NULL;
	te.init();
	te.load_font(FONT, FONT_SIZE);
}

void game_engine::add_sprite(game_sprite* spr, int x, int y, int layer)
{
	eng_sprite s;
	s.states = templates.sprites[spr->path];
	s.x = x;
	s.y = y;
	s.z = layer/100.0f;
	s.state = "walking";
	s.frame = 0;
	s.tex = textures[mapped_textures[spr->path]];
	sprites.push_back(s);
	build_sprites();
}

void game_engine::init() {
	projection = glm::ortho( 0.f, float(screen_w), float(screen_h), 0.0f, 0.0f, 100.f );
	view = glm::lookAt(
		glm::vec3(0,0,1), // Camera is at (0,0,5), in World Space
		glm::vec3(0,0,0), // and looks at the origin
		glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

void game_engine::load_project(string base_dir)
{

}

shader_program game_engine::make_shader(std::vector<string> files) {
	auto sp = shader_program();
	for (int i = 0; i < files.size(); i++)
	{
		GLenum shader_type = util::endswith(files[i], ".vertex") ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
		sp.add_shader(game_loader->get_shader(files[i]).c_str(), files[i].c_str(), shader_type);
	}
	sp.link_shaders();
	return sp;
}

// eng_texture game_engine::make_tmap_texture()
// {
// 	build_cache(cfg::TILE_SIZE, cfg::mag);
// }

vbo game_engine::prepare_for(game_map mymap) {
	auto vb = gen::vertex_grid(20, 15, 1);
	//printf("vb[0]=%f\n", vb[3]);
    auto gt = data::flatten_layer(mymap, 0);
    auto tc = gen::texture_map(gt, game_loader);

    //printf("dt[5]=%f\n", dt[5]);
    auto vertex_data = gen::intercalate<3,2>(vb, tc);

    vbo tile_buffer;
    tile_buffer.init(GL_DYNAMIC_DRAW);
	    tile_buffer.buffer(vertex_data);
	    tile_buffer
	    	.add_pointer("position", 3, GL_FLOAT)
	    	.add_pointer("tex_coord", 2, GL_FLOAT)
	    .attach(tile_buffer);
	return tile_buffer;
}

void game_engine::build_sprites() {
	auto vb = gen::sprite_vertex(sprites, 2);
	//printf("vb[0]=%f\çn", vb[3]);
    auto tc = gen::sprite_texture_map(sprites, textures);

    //printf("dt[5]=%f\n", dt[5]);
    auto vertex_data = gen::intercalate<3,2>(vb, tc);

    sprite_vbo.init(GL_DYNAMIC_DRAW);
	    sprite_vbo.buffer(vertex_data);
	    sprite_vbo
	    	.add_pointer("position", 3, GL_FLOAT)
	    	.add_pointer("tex_coord", 2, GL_FLOAT)
	    .attach(sprite_vbo);
}

void game_engine::setup(int w, int h)
{
	setup_linux();
	setup_gui(w ,h);
}

void game_engine::setup_linux()
{
	init_crt();
	setup_stdout();
	setup_threads();
}

void game_engine::setup_gui(int w, int h)
{
	glx::setup_x(w, h);
	glx::init_glew();
	glx::init_gl(w, h);
}

void game_engine::draw_text(string text)
{

}

bool point_in_rect(int x, int y, eng_coord r)
{
	// printf("rect: %d, %d, %d, %d | point: %d, %d\n",
	// 	r.x,r.y,r.w,r.h,x,y);

	return (
		x >= r.x &&
		x <= r.x+r.w &&
		y >= r.y &&
		y <= r.y+r.h);
}

eng_object::eng_object(string name)
{
	this->name = name;
}

void game_engine::click_event(int x, int y)
{
	if (objects.size() == 0) return;
	std::vector<eng_object*> possible;
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		auto cur_obj = it->second;
		if (point_in_rect(x,y, {cur_obj.x,cur_obj.y,cur_obj.w,cur_obj.h}))
			possible.push_back(&(objects[it->first]));
		//printf("object %d: %s\n", i, objects[i].name.c_str());
	}
	//printf("possibles %d\n", possible.size());
	if (possible.size() == 0)
	{
		selected = NULL;
		return;
	}
	eng_object* o = &(objects.begin()->second);
	for (auto i = 1; i < possible.size(); i++)
	{
		if (o->z < possible[i]->z) o = possible[i];
	}
	o->click_event(x, y);
}

void eng_object::click_event(int x, int y)
{
	click_function(this, x ,y);
}

eng_object::eng_object()
{

}

coord_grid game_engine::texture_viewer(eng_texture txt, string name, int x, int y, int z)
{
	eng_object o(name);
	o.x = x;
	o.y = y;
	o.w = txt.w;
	o.h = txt.h;
	o.z = z/100.0f;
	objects[name] = o;
	//printf("object size %d\n", objects.size());
	return gen::texview(txt,9);
}

bool game_engine::tick(int val)
{
	auto new_frame = false;
	for (auto it = sprites.begin(); it != sprites.end(); it++)
	{
		if ((*it).tick(val) == true) new_frame = true;
	}
	return new_frame;
}

path_map<seq_piece_t> by_image(path_map<map<string, seq_piece_t>> input)
{
	path_map<seq_piece_t> retval;
	for (auto at = input.begin(); at != input.end(); at++)
		for (auto act = at->second.begin(); act != at->second.end(); act++)
			for (auto fr = act->second.begin(); fr != act->second.end(); fr++)
			{
		 		if (retval.count((*fr).img) == 0)
				{
					retval[(*fr).img] = seq_piece_t();
				}
				retval[(*fr).img].push_back((*fr));
			}
	return retval;
}

void game_engine::blit_atlas(path_map<seq_piece_t> input)
{
	for (auto at = input.begin(); at != input.end(); at++)
	{
			dbgprint("selecting image %s for blitting", at->first.c_str());
			auto src = tex_man->get_texture(at->first);
			//auto dst = eng_texture::blank_texture(1024, 1024);

			for (auto fr_iter = at->second.begin(); fr_iter != at->second.end(); fr_iter++) {
				auto fr = *fr_iter;
				src.blit(src, fr.ox, fr.oy, fr.w, fr.h, fr.x, fr.y);
			}
	}

}

std::vector<eng_texture> game_engine::make_atlas(std::vector<string> paths)
{
	// add all assets
	atlas_builder ab;
	for (auto path = paths.begin(); path != paths.end(); path++)
	{
		dbgprint("%s", (*path).c_str());
		auto p_type = game_loader->resolve_type(*path);

		if (p_type == "SPR")
		{
			dbgprint("adding sprite %s", (*path).c_str());
			ab.add(game_loader->get_sprite_ptr(*path));
		} else if (p_type == "TIL")
		{
			dbgprint("adding tile   %s", (*path).c_str());
			ab.add(game_loader->get_tileset_ptr(*path));
		} else {
			// error
		}
	}

	// pack in N atlasses where N is the minimum possible number of rectangles
	// needed to compose the added assets
	dbgprint("before pack");
	//return vector<eng_texture>();
	auto a_def = ab.compile(29, 1024);
	blit_atlas(by_image(a_def));

	dbgprint("packed");
	/*
	for (auto at = atlas.begin(); at != atlas.end(); at++)
	{
		auto p_type = game_loader->resolve_type(at->first);
		if (p_type == "SPR")
		{
			for (auto act = at->second.begin(); act != at->second.end(); act++)
			{
				templates.sprites[at->first][act->first] = act->second;
				if ((*pcs).act == "")
				{
					auto qc = ab.ap_to_qc(cur_tile);
					// add to templates
					//cur_tile.
					//templates.tiles[path][cur_tile.tile_id].push_back(qc);
					printf("tile packed\n");
					//game_loader->get_sprite_ptr(path)->states[cur_tile.act]
					//ab.add(game_loader->get_sprite_ptr(*path));
				} else
				{
					printf("Piece %0d\nx: %d y: %d w: %d h: %d\n", cur_tile.tile_id,
							cur_tile.x, cur_tile.y, cur_tile.w, cur_tile.h );
				}
			}
		}
	}*/
	//dbgprint("done","done");
	return vector<eng_texture>();
}

eng_texture game_engine::blank_texture(int w, int h)
{
	auto tex = eng_texture::blank_texture(w, h);
	textures[tex.texture_id] = tex;
	return tex;
}
