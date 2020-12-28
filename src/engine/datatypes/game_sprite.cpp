#include <nlohmann/json.hpp>
#include "game_sprite.h"

using json = nlohmann::json;

game_sprite game_sprite::from_json(string j, string path) {
	auto json = json::parse(j);
	auto retval = game_sprite();
	retval.path = path;
	retval.name = json["name"];
	retval.id = json["id"];

	for (auto it = json["states"].begin();
		it != json["states"].end(); it++) {
		auto spr = std::vector<sprite_frame>();
		auto index = 0;
		for (auto fr = (*it).begin(); fr != (*it).end(); fr++, index++) {
			auto tmp = sprite_frame();
			tmp.img = (*fr)["img"];
			tmp.u = (*fr)["u"];
			tmp.v = (*fr)["v"];
			tmp.w = (*fr)["w"];
			tmp.h = (*fr)["h"];
			tmp.ox = (*fr)["ox"];
			tmp.oy = (*fr)["oy"];
			tmp.index = index;
			tmp.interval = (*fr)["interval"];
			for (auto hb = (*fr)["hitbox"].begin(); hb != (*fr)["hitbox"].end(); hb++) {
				auto cur_hb = hitbox();
				cur_hb.x = (*hb)["x"];cur_hb.w = (*hb)["w"];
				cur_hb.y = (*hb)["y"];cur_hb.h = (*hb)["h"];
				tmp.hitboxes.push_back(cur_hb);
			}
			spr.push_back(tmp);
		}
		retval.states[it.key()] = spr;
	}
	return retval;
}

std::vector<sprite_frame> game_sprite::get_frame_array()
{
	std::vector<sprite_frame> retval;
	for (auto it = states.begin(); it != states.end(); it++)
	{
		retval.insert(retval.end(), it->second.begin(), it->second.end());
	}
	return retval;
}
