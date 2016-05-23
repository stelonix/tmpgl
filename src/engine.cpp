#include "assets.h"
#include "engine.h"
#include "generators.h"
#include "helpers.h"

#include "cfg.h"
using namespace cfg;

extern asset_loader* a_loader;

game_engine::game_engine()
{
	selected = NULL;
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
    tile_buffer.init();
	    tile_buffer.buffer_data(vertex_data);
	    tile_buffer
	    	.add_pointer("position", 3, GL_FLOAT)
	    	.add_pointer("tex_coord", 2, GL_FLOAT)
	    .attach(tile_buffer);
	return tile_buffer;
}