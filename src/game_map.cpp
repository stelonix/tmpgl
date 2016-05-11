#include <tuple>
#include "assets.h"
#include "game_map.h"
#include "include/json.hpp"
#include "assets.h"

using json = nlohmann::json;
using namespace std;

game_map::game_map()
{

}

game_map::game_map(string name, int w, int h, int l) {
	this->name = name;
	set_layers(l);
	resize(w, h);
};

void game_map::set_layers(int l) {
	layers.resize(l);
}

void game_map::set_tile(int layer, int x, int y, map_tile tile) {
	layers[layer].tiles[x+y*w] = tile;
}

void game_map::resize(int w, int h) {
	this->w = w;
	this->h = h;
	for (auto i = 0; i < layers.size(); i++) {
		layers[i].tiles.resize(w*h);
	}
}

map_tile game_map::get_tile(int layer, int x, int y) {
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
extern asset_loader* a_loader;

res_map<game_tileset> flatten_tilesets(std::vector<string> tsets) {
	auto retval = res_map<game_tileset>();
	for (auto i = 0; i < tsets.size(); i++)
	{
		retval[i] = a_loader->loaded_tilesets[tsets[i]];
	}
	return retval;
}

game_tilemap game_map::flatten_layer(int z)
{
	auto tiles = layers[z].tiles;
	auto retval = game_tilemap();
	auto tmaps = flatten_tilesets(tilesets);
	for (auto i = 0; i < tiles.size(); i++) {
		retval.push_back(	tmaps[get<0>(tiles[i])].tiles
								 [get<1>(tiles[i])]);

	}
	return retval;
}

/*eng_texture game_map::tex_from_map_tile(map_tile tile) {
	auto tex_name = tilesets[get<0>(tile)];
	return a_loader->loaded_text[tex_name];
}*/