typedef std::tuple<int, int> tile_t;
typedef std::vector<tile_t> tilemap;

struct map_layer {
//	std::string name
};

struct map_decal {
	std::string name;
	std::string sprite;
	int x;
	int y;
	int z;
};

struct game_map {
	std::string name;
	int w;
	int h;
	std::vector<tilemap> layers;
	std::vector<tileset> tilesets;
	std::vector<map_decal> objects;

	game_map(std::string name, int w, int h) {
		this->name = name;
		resize(w, h);
	};

	void set_tile(int layer, int x, int y, tile_t tile) {
		layers[layer][x+y*w] = tile;
	}

	void resize(int w, int h) {
		this->w = w;
		this->h = h;
		for (auto i = 0; i < layers.size(); i++) {
			layers[i].resize(w*h);
		}
	}

	tile_t get_tile(int layer, int x, int y) {
		return layers[layer][x+y*w];
	}
};