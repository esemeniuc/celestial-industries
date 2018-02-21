#pragma once

#include <fstream>
#include "tile.hpp"
#include <map>
#include <tuple>


struct TimeTile {
	OBJ::Data present;
	OBJ::Data past;
};

enum TileType {
	SAND_1,
	SAND_2,
	SAND_3,
	WALL,
	BRICK_CUBE,
	MINING_TOWER,
	PHOTON_TOWER,
};

class Level
{
public:
	bool init(std::vector<std::vector<int>> intArray, std::vector<std::tuple<TileType, std::string>> sources);
	std::vector<std::vector<int>> levelLoader(std::string levelTextFile);

	std::vector<std::vector<Tile>> tiles; // we can add the time dimension when we get there
	std::map<TileType, OBJ::Data> tileTypes;
private:
	bool initTileTypes(std::vector<std::tuple<TileType, std::string>> sources);
};