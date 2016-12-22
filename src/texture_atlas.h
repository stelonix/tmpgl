
struct atlas_piece {
	string img;
	int x, y, w, h;
};

struct texture_atlas {
	void add(sprite_frame frame);
	void add(int x, int y, int w, int h);
};