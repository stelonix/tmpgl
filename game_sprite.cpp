#include "game_sprite.h"
#include "include/json.hpp"
using json = nlohmann::json;

game_sprite game_sprite::from_json(std::string j) {
	auto json = json::parse(j);
	auto retval = game_sprite();
	retval.name = json["name"];
	retval.id = json["id"];

	for (auto it = json["states"].begin();
		it != json["states"].end(); it++) {
		auto spr = sprite_anim();
		for (auto fr = (*it).begin(); fr != (*it).end(); fr++) {
			auto tmp = sprite_frame();
			tmp.u = (*fr)["u"];
			tmp.v = (*fr)["v"];
			tmp.w = (*fr)["w"];
			tmp.h = (*fr)["h"];
			tmp.ox = (*fr)["ox"];
			tmp.oy = (*fr)["oy"];
			tmp.interval = (*fr)["u"];
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

