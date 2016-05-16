#include "assets.h"
#include "engine.h"
#include "helpers.h"

#include "cfg.h"
using namespace cfg;

extern asset_loader* a_loader;

void engine::load_shaders() {
	for (string s : list_files(SHADER_DIR)) {
		a_loader->load_shader(SHADER_DIR+"/"+s);
	}
}