#include <tuple>
#include <string.h>
#include "cfg.h"
#include "game_map.h"
#include "include/json.hpp"

using json = nlohmann::json;
using namespace cfg;
using namespace std;

game_map::game_map(string name, int w, int h, int l) {
	this->name = name;
	set_layers(l);
	resize(w, h);
};

void game_map::set_layers(int l) {
	layers.resize(l);
}

void game_map::set_tile(int layer, int x, int y, tile_t tile) {
	layers[layer].tiles[x+y*w] = tile;
}

void game_map::resize(int w, int h) {
	this->w = w;
	this->h = h;
	for (auto i = 0; i < layers.size(); i++) {
		layers[i].tiles.resize(w*h);
	}
}

tile_t game_map::get_tile(int layer, int x, int y) {
	return layers[layer].tiles[x+y*w];
}

game_map game_map::from_json(string j) {
	auto json = json::parse(j);
	auto retval = game_map(	json["name"],
							json["w"], json["h"],
							json["layers"].size());
	auto ts_node = json["tilesets"];
	for (auto it = ts_node.begin(); it != ts_node.end(); it++) {
		retval.tilesets.push_back(*it);
	}
	auto l_node = json["layers"];
	for (auto it = l_node.begin(); it != l_node.end(); it++) {
		auto cln = l_node[it.key().c_str()];
		auto tmp = map_layer();
			tmp.name = it.key();
			tmp.z = cln["z"];

		for (auto i = 0; i < cln["tiles"].size(); i++) {
			auto ct = cln["tiles"][i];
			tmp.tiles.push_back(std::make_tuple(ct[0], ct[1]));
		}
		for (auto ti = cln["objects"].begin();
					ti != cln["objects"].end(); ti++) {
			auto cur_dec = cln["objects"][ti.key()];
			auto dec = map_decal();
				dec.name = ti.key();
				dec.sprite = cur_dec["sprite"];
				dec.x = cur_dec["x"];
				dec.y = cur_dec["y"];
			tmp.objects.push_back(dec);
		}
		retval.layers.push_back(tmp);
	}
	return retval;
}


std::vector<float> gen_map_sh(int w, int h, int l) {
	const int PER_VERTEX = 5;
	const int NUM_VERTEX = 6;
	const int NUM_ELEMENTS = NUM_VERTEX*PER_VERTEX;
	const int SIZE_VEC = w*h*NUM_ELEMENTS;
	const float UNDF = numeric_limits<float>::quiet_NaN();
	std::vector<float> retval;
	auto z = l/100.0f;
	retval.resize(SIZE_VEC);
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			float vertices[] ={
				x*TILE_SIZE,		y*TILE_SIZE,		z,
					UNDF, 				UNDF,
				(x+1)*TILE_SIZE,	y*TILE_SIZE,		z,
					UNDF, 				UNDF,
				(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	z,
					UNDF, 				UNDF,
				x*TILE_SIZE,		y*TILE_SIZE,		z,
					UNDF, 				UNDF,
				x*TILE_SIZE,		(y+1)*TILE_SIZE,	z,
					UNDF, 				UNDF,
				(x+1)*TILE_SIZE,	(y+1)*TILE_SIZE,	z,
					UNDF, 				UNDF
			};
			memcpy(retval.data()+(y*w+x)*NUM_ELEMENTS,
					vertices, sizeof(vertices));
		}
	}
	return retval;	
}