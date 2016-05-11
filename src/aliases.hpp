#ifndef ALIASES_HPP
#define ALIASES_HPP

#include "string"
#include <array>
#include <map>
#include <tuple>
#include <vector>

// forward declarations

struct sprite_frame;
struct game_tile_frame;

// actual types

typedef std::vector<float> coord_grid;
typedef std::vector<game_tile_frame> game_tile;
typedef std::vector<game_tile> game_tilemap;
typedef std::tuple<int, int> map_tile;
typedef std::vector<map_tile> map_tilemap;
template<typename T>
	using path_map = std::map<std::string, T>;
template<typename T>
	using res_map = std::map<int, T>;
typedef std::vector<sprite_frame> sprite_anim;
typedef std::array<float,2> uv_coord;

#endif