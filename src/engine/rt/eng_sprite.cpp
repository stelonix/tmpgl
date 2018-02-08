#include "eng_sprite.h"

bool eng_sprite::tick(int val)
{

	auto retval = false;
	auto c_spr = states[state];
	//printf("sprite tick %d into %d/%d\n", val, cur_tick, c_spr[frame].interval);
	if (c_spr.size() == 1) return false;
	cur_tick += val;
	if (cur_tick > c_spr[frame].interval) {
		cur_tick = cur_tick % c_spr[frame].interval;
		frame++;
		if (frame >= c_spr.size()) {
			frame = 0;
		}
		retval = true;
	}

	return retval;
}

eng_sprite::eng_sprite()
{
	cur_tick = 0;
}