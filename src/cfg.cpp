#include "cfg.h"

float cfg::ATILE = 16.0f;
int cfg::MAG = 2;
float cfg::TILE_SIZE = ATILE * MAG;
string cfg::ASSETS_DIR("./assets/");
string cfg::SHADER_DIR(ASSETS_DIR + "./shaders"_s);
int cfg::HORZ_RES = 800;
int cfg::VERT_RES = 600;