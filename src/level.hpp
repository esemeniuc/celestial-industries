#pragma once

#include <limits>
#include <ostream> //for overloaded << operator
#include <map>
#include "common.hpp"
#include "tile.hpp"

#define INF std::numeric_limits<float>::infinity()

struct TimeTile {
	OBJ::Data present;
	OBJ::Data past;
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

class Level {
public:
	//members
	// Using a shared pointer to a tile allows us to actually have derived classes in there as well.
	std::vector<std::vector<std::shared_ptr<Tile>>> tiles; // we can add the time dimension when we get there

	//funcs
	bool init(
			std::vector<std::vector<Config::MeshType>> levelArray,
			std::map<Config::MeshType, std::shared_ptr<Renderer>> meshRenderers
	);

	void update(float ms);

	bool displayPath(const std::vector<Coord>& levelArray);

	std::vector<std::vector<Config::MeshType>> levelLoader(const std::string& levelTextFile);

	std::vector<std::vector<AStarNode>> getLevelTraversalCostMap();

private:
	//members
	std::vector<std::vector<AStarNode>> levelTraversalCostMap;

	//funcs
};
