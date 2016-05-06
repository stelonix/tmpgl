#include "include/json.hpp"
#include "tileset.h"

using json = nlohmann::json;

tileset tileset::from_json(string j) {
	auto json = json::parse(j);
	auto retval = tileset();
	retval.name = json["name"];
	for (auto it = json["tiles"].begin();it != json["tiles"].end(); it++) {
		auto key = (*it)["id"];
		retval.tiles[key] = game_tile();

		for (auto fr = (*it)["frames"].begin();
					fr != (*it)["frames"].end(); fr++) {
			auto cf = tile_frame();

			cf.img = (*fr)["image"];

			cf.interval = (*fr)["interval"];
			cf.u = (*fr)["u"];
			cf.v = (*fr)["v"];

			cf.pass = (*fr)["pass"];

			retval.tiles[key].push_back(cf);
		}
	}

	return retval;
}