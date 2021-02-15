#ifndef ATLAS_PIECE_H
#define ATLAS_PIECE_H

#include <string>
using namespace std;
#include "engine/rt/eng_types.h"

struct atlas_piece : public eng_rect {
	string act, img, src;
	int index, tile_id, ox, oy;
};

#endif
