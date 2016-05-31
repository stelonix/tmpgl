#include "assets.h"
#include "engine.h"
#include "generators.h"
#include "helpers.h"

#include "cfg.h"
using namespace cfg;

extern asset_loader* a_loader;

#define FONT "./Sevastopol-Interface.ttf"
#define FONT_SIZE 36

game_engine::game_engine()
{
	selected = NULL;
	te.init();
	te.load_font(FONT, FONT_SIZE);
}

void game_engine::load_shaders() {
	for (string s : list_files(SHADER_DIR)) {
		a_loader->load_shader(SHADER_DIR+"/"+s);
	}
}

vbo game_engine::prepare_for(game_map mymap) {
	auto vb = gen::vertex_grid(20, 15, 1);
	//printf("vb[0]=%f\n", vb[3]);
    auto gt = mymap.flatten_layer(0);
    auto tc = gen::texture_map(gt, a_loader);
    
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

void game_engine::draw_text(string text)
{

}

bool point_in_rect(int x, int y, rect r)
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
	for (auto i = 0; i < objects.size(); i++)
	{
		auto cur_obj = objects[i];
		if (point_in_rect(x,y, {cur_obj.x,cur_obj.y,cur_obj.w,cur_obj.h}))
			possible.push_back(&(objects[i]));
		//printf("object %d: %s\n", i, objects[i].name.c_str());
	}
	//printf("possibles %d\n", possible.size());
	if (possible.size() == 0)
	{
		selected = NULL;
		return;
	}
	eng_object* o = &(objects[0]);
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
	objects.push_back(o);
	//printf("object size %d\n", objects.size());
	return gen::texview(txt,9);
}