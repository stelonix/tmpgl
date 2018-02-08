#ifndef ENG_TYPES_H
#define ENG_TYPES_H

struct eng_coord {
	int x;int y;int w; int h;
};
struct quad_coords {
	eng_coord coords;
    int interval;
    int tex;
};

#endif
