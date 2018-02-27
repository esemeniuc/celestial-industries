#pragma once

#include <fstream>
#include <map>
#include <limits>
#include <ostream>
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
    GUN_TURRET,
};

// used to build a graph of nodes for the AI pathfinder to traverse each tile node.
struct AStarNode {
	int rowCoord, colCoord, movementCost;
	float fScore;

	AStarNode() = default;

	AStarNode(int _rowCoord, int _colCoord, int _movementCost, float _fScore) : rowCoord(_rowCoord),
																						colCoord(_colCoord),
																						movementCost(_movementCost),
																						fScore(_fScore) {}

	bool operator==(const AStarNode& rhs) const {
		return rowCoord == rhs.rowCoord &&
			   colCoord == rhs.colCoord;
	}

	bool operator!=(const AStarNode& rhs) const {
		return !(rhs == *this);
	}

	friend std::ostream& operator<<(std::ostream& os, const AStarNode& state) {
		os << "rowCoord: " << state.rowCoord << " colCoord: " << state.colCoord << " movementCost: "
		   << state.movementCost << " fScore: " << state.fScore;
		return os;
	}
};

struct Coord {
	int rowCoord, colCoord;

	Coord() = default;

	Coord(AStarNode a) : rowCoord(a.rowCoord), colCoord(a.colCoord) {}

	Coord(int _rowCoord, int _colCoord) : rowCoord(_rowCoord), colCoord(_colCoord) {}
};

class Level {
public:
	//members
    // Using a shared pointer to a tile allows us to actually have derived classes in there as well.
	std::vector<std::vector<std::shared_ptr<Tile>>> tiles; // we can add the time dimension when we get there
	std::map<TileType, std::vector<SubObject>> tileTypes;
    std::map<TileType, std::shared_ptr<CompositeObjectBulkRenderer>> tileRenderers;

	//funcs
	bool init(
			std::vector<std::vector<TileType>> intArray,
			std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources,
			std::shared_ptr<Shader> shader
	);

	void update(float ms);

    std::vector<std::vector<TileType>> levelLoader(const std::string& levelTextFile);
    std::vector<std::vector<AStarNode>> getLevelTraversalCostMap();
	bool displayPath(const std::vector<Coord>& levelArray);
private:
	//members
	std::vector<std::vector<AStarNode>> levelTraversalCostMap;

	//funcs
	bool initTileTypes(std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources);
};