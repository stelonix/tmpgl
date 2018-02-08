#include <algorithm>
#include "texture_atlas.h"
#include "cfg.h"
#define STB_RECT_PACK_IMPLEMENTATION
#include "include/imgui/stb_rect_pack.h"

void atlas_builder::add(sprite_frame frame, string act, string src)
{
	atlas_piece p;
	p.x = frame.u;
	p.y = frame.v;
	p.w = frame.w;
	p.h = frame.h;
	p.img = frame.img;
	p.src = src;
	p.index = frame.index;
	p.act = act;
	//input_pieces.push_back(p);
	printf("src: %s act: %s index: %d\n", src.c_str(), act.c_str(), frame.index);
	requests[src][act].push_back(p);
	printf("added piece\n");
}

void atlas_builder::add(string img, int u, int v, int w, int h, int tile_id, int index, string src)
{
	atlas_piece p;
	p.x = u;
	p.y = v;
	p.w = w;
	p.h = h;
	p.img = img;
	p.src = src;
	p.index = index;
	p.tile_id = tile_id;
	//input_pieces.push_back(p);
	requests[src][to_string(tile_id)][index] = p;
}

void atlas_builder::add(game_sprite* spr)
{
	printf("adding sprite %s\n", spr->name.c_str());
	if (requests.count(spr->path) == 0) requests[spr->path] = map<string, seq_piece_t>();
	for (auto act = spr->states.begin(); act != spr->states.end(); act++)
	{
		printf("act: %s\n", act->first.c_str());
		if (requests.count(spr->path) == 0) requests[spr->path][act->first] = seq_piece_t();
		auto frames = act->second;
		for (auto fr = frames.begin(); fr != frames.end(); fr++)
		{
			add(*fr, act->first, spr->path);
		}
	}
	printf("done adding sprite\n");
}

void atlas_builder::add(game_tileset* ts)
{
	printf("adding tile\n");
	for (auto t = ts->tiles.begin(); t != ts->tiles.end(); t++)
	{
		auto frames = t->second; auto index = 0;
		for (auto f_iter = frames.begin(); f_iter != frames.end(); f_iter++, index++)
		{
			printf("adding frame\n");
			auto f = *f_iter;
			add(f.img, f.u, f.v, cfg::ATILE, cfg::ATILE, t->first, index, ts->path);
		}
	}
	printf("done adding tile\n");
}

vector<stbrp_rect> atlas_builder::ap_to_stb(vector<atlas_piece>& piece_arr, int& _in_id)
{
	vector<stbrp_rect> retval;
	for (auto it = piece_arr.begin(); it != piece_arr.end(); it++)
	{
		auto piece = *it;
		stbrp_rect cur;
		cur.id = _in_id;
		cur.w = piece.w;
		cur.h = piece.h;
		retval.push_back(cur);
		//printf("PIECE DATA\nw: %d\nh:%d", cur.w, cur.h);
		_in_id++;
	}
	return retval;
}

quad_coords atlas_builder::ap_to_qc(atlas_piece piece)
{
	quad_coords retval;
	retval.coords.x = piece.x;
	retval.coords.y = piece.y;
	retval.coords.w = piece.w;
	retval.coords.h = piece.h;
	return retval;
}

vector<atlas_piece> atlas_builder::stb_to_ap(vector<stbrp_rect>& rects)
{
	vector<atlas_piece> retval;
	for (auto it = rects.begin(); it != rects.end(); it++)
	{
		//if (i != cur.id) continue;
		auto rect = *it;
		auto piece = input_pieces[rect.id];
		piece.x = rect.x;
		piece.y = rect.y;
		retval.push_back(piece);
	}
	return retval;
}

vector<texture_atlas> atlas_builder::pack(int max_w, int max_h)
{
	stbrp_context context;
	vector<stbrp_rect> tmp_rects;
	auto num_nodes = max_w * 2;
	//auto returns = requests;
	struct stbrp_node* nodes = new stbrp_node[num_nodes];
	stbrp_init_target(&context, max_w, max_h, nodes, num_nodes);
	auto id = 0;
	for (auto s : requests) {
		for (auto pcs : s.second) {
			auto rects = ap_to_stb(pcs.second, id);
			do
			{
				//printf("packing...\n");
				stbrp_pack_rects(&context, rects.data(), rects.size());
				tmp_rects.insert(tmp_rects.begin(), rects.begin(), rects.end());
				auto old_sz = rects.size();
				rects = remove_packed(rects);
				// do we need a new atlas?
				if (rects.size() == old_sz) {
				//	stbrp_init_target(&context, max_w, max_h, nodes, num_nodes);
				//	abort();
				}
				//stbrp_init_target(&context, max_w, max_h, nodes, num_nodes);
			} while (rects.size() != 0);
		}
	}
	delete[] nodes;

	// sort so we have ids back


	std::sort(tmp_rects.begin(), tmp_rects.end(),[&](const stbrp_rect & a, const stbrp_rect & b) -> bool
	{
		return a.id < b.id;
	});
	id = 0;
	for (auto s : requests) {
		for (auto pcs : s.second) {
			requests[s.first][pcs.first][id].x = tmp_rects[id].x;
			requests[s.first][pcs.first][id].y = tmp_rects[id].y;
			id++;
		}
	}

	// output logical texture atlases
	vector<texture_atlas> retval;
	texture_atlas atlas;
	//atlas.pieces = tmp_rects;
	//retval.push_back(atlas);


	return retval;
}

void texture_atlas::calc_dimensions()
{
	w = 0; h = 0;
	for (auto it = pieces.begin(); it != pieces.end(); it++)
	{
		auto r = *it;
		if (r.x + r.w > w) w = r.x+r.w;
		if (r.y + r.h > h) h = r.y+r.h;
	}
}

vector<stbrp_rect> atlas_builder::remove_packed(vector<stbrp_rect> rects)
{
	//printf("size before %d\n", rects.size());
	rects.erase(std::remove_if(rects.begin(), rects.end(),
		[&](const stbrp_rect& r)
		{
			//printf("%s\n", r.was_packed ? "true" : "false");
			return r.was_packed;
		})
	);
	//printf("size after %d\n", rects.size());
	return rects;
}
