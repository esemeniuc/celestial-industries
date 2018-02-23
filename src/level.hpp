#pragma once

#include <fstream>
#include <map>
#include <limits>
#include "tile.hpp"

#define INF std::numeric_limits<float>::infinity()

struct TimeTile {
	OBJ::Data present;
	OBJ::Data past;
};

enum class TileType {
	SAND_1,
	SAND_2,
	SAND_3,
	WALL,
	BRICK_CUBE,
	MINING_TOWER,
	PHOTON_TOWER,
	TREE,
};

// used to build a graph of nodes for the AI pathfinder to traverse each tile node.
struct aStarPathState
{
	int rowCoord, colCoord, currentPathCost;
	float fScore;

	aStarPathState() = default;

	aStarPathState(int _rowCoord, int _colCoord, int _currentPathCost, float _fScore) : rowCoord(_rowCoord),
																					   colCoord(_colCoord),
																					   currentPathCost(_currentPathCost),
																					   fScore(_fScore) {}

	bool operator==(const aStarPathState& rhs) const {
		return rowCoord == rhs.rowCoord &&
			   colCoord == rhs.colCoord &&
			   currentPathCost == rhs.currentPathCost &&
			   fScore == rhs.fScore;
	}

	bool operator!=(const aStarPathState& rhs) const {
		return !(rhs == *this);
	}
};

class Level {
public:
	//members
	std::vector<std::vector<Tile>> tiles; // we can add the time dimension when we get there
	std::map<TileType, OBJ::Data> tileTypes;

	//funcs
	bool init(const std::vector<std::vector<TileType>>& intArray,
			  const std::vector<std::pair<TileType, std::string>>& sources);

	bool displayPath(const std::vector<aStarPathState>& levelArray);

	std::vector<std::vector<TileType>> levelLoader(const std::string& levelTextFile);

	std::vector<std::vector<aStarPathState>> getLevelTraversalCostMap();

private:
	//members
	std::vector<std::vector<aStarPathState>> levelTraversalCostMap;

	//funcs
	bool initTileTypes(const std::vector<std::pair<TileType, std::string>>& sources);
};