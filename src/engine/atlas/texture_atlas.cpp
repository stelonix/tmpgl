#include "texture_atlas.h"

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
