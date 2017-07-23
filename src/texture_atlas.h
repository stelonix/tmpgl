#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H
#include <tuple>
#include "string"
#include "eng_types.h"
#include "game_sprite.h"
#include "game_tileset.h"
#include "include/imgui/stb_rect_pack.h"

using namespace std;

struct atlas_piece : public eng_coord {
	string act, img, src;
	int index, tile_id;
};

struct texture_atlas {
	// data
	int w, h;
	std::map<string, atlas_piece* const> by_file;
	vector<atlas_piece> pieces;
	void calc_dimensions();
};

typedef vector<atlas_piece> seq_piece_t;

struct atlas_builder {
	vector<atlas_piece> input_pieces;
	//requests[src_file][identifier][frame_index]

	map<string, map<string, seq_piece_t>> requests; // we store tile ids a string
	void add(sprite_frame frame);

	void add(sprite_frame frame, string act, string src);

	void add(string img, int u, int v, int w, int h, int tile_id, int index, string src);
	void add(game_sprite* spr);
	void add(game_tileset* ts);
	vector<stbrp_rect > ap_to_stb(vector<atlas_piece>& piece_arr, int& _in_id);
	quad_coords ap_to_qc(atlas_piece piece);
	vector<atlas_piece> stb_to_ap(vector<stbrp_rect>& rects);
	vector<texture_atlas> pack(int max_w, int max_h);
	vector<stbrp_rect> remove_packed(vector<stbrp_rect> rects);
};

#endif
