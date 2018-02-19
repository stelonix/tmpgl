#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
#include <map>
#include <tuple>
#include <vector>
#include "engine/atlas/atlas_piece.h"

using namespace std;

struct texture_atlas {
	// data
	int w, h;
	map<string, atlas_piece* const> by_file;
	vector<atlas_piece> pieces;
	void calc_dimensions();
};

#endif
