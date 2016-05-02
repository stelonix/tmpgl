#include <map>
#include <string>
#include <vector>

struct hitbox {
	int x,y,w,h;
};

struct sprite_frame {
	int u,v,w,h,ox,oy;
	int interval;
	std::vector<hitbox> hitboxes;
};

struct game_sprite {
	std::string name;
	std::map<std::string,sprite_frame> states;
};