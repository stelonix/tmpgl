#include <map>
#include <string>
#include <vector>

struct hitbox {
	int x,y,w,h;
};

struct sprite_frame {
	std::string img;
	int u,v,w,h,ox,oy;
	int interval;
	std::vector<hitbox> hitboxes;
};

typedef std::vector<sprite_frame> sprite_anim;

struct game_sprite {
	int id;
	std::string name;
	std::map<std::string, sprite_anim> states;
	static game_sprite from_json(std::string j);
};