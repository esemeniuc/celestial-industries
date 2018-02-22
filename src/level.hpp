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

// used to build a graph of nodes for the AI pathfinder to traverse
// each tile node defines a tile row, column and cost in the grid
typedef std::tuple<long, long, float, float> tileNode;

class Level
{
public:
	bool init(std::vector<std::vector<int>> intArray, std::vector<std::tuple<TileType, std::string>> sources);
	std::vector<std::vector<int>> levelLoader(std::string levelTextFile);
	std::vector<std::vector<Tile>> tiles; // we can add the time dimension when we get there
	std::map<TileType, OBJ::Data> tileTypes;
	std::vector < std::vector<tileNode>> getLevelTraversalCostMap();

private:
	bool initTileTypes(std::vector<std::tuple<TileType, std::string>> sources);
	std::vector<std::vector<tileNode>> levelTraversalCostMap;
};