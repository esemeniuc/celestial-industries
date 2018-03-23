#pragma once

#include <limits>
#include <ostream> //for overloaded << operator
#include <map>
#include "common.hpp"
#include "config.hpp"
#include "tile.hpp"
#include "model.hpp"
#include "entity.hpp"

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

	AStarNode(int _colCoord, int _rowCoord, int _movementCost, float _fScore) : rowCoord(_rowCoord),
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
	std::vector<std::shared_ptr<Tile>> tiles; // we can add the time dimension when we get there
	std::vector<std::shared_ptr<Entity>> entities;
	std::shared_ptr<Tile> tileCursor;
	//funcs
	bool init(
			std::vector<std::vector<Model::MeshType>>& levelArray,
			std::vector<std::shared_ptr<Renderer>>& meshRenderers
	);

	void update(float ms);

	bool displayPath(const std::vector<Coord>& levelArray);

	std::vector<std::vector<Model::MeshType>> levelLoader(const std::string& levelTextFile);

	std::vector<std::vector<AStarNode>> getLevelTraversalCostMap();

	// Places a tile, replaceing anything there before. If the tile is larger than standard specify the width and height.
	// The location reffer's to the tile's top left corner (0,0,0) being the minimum accepted. The location is NOT the center of the tile.
	std::shared_ptr<Tile> placeTile(Model::MeshType type, glm::vec3 location, unsigned int width=1, unsigned int height=1);

	std::shared_ptr<Entity> placeEntity(Model::MeshType type, glm::vec3 location);

	std::shared_ptr<Tile> tileFromMeshType(Model::MeshType type);

	std::shared_ptr<Entity> entityFromMeshType(Model::MeshType type);

	Coord getLevelSize() const;

private:
	//members
	std::vector<std::vector<AStarNode>> levelTraversalCostMap;

	// Indexable using MeshType enum
	std::map<Model::MeshType, std::pair<double, float>> tileToCost;

	//funcs
};
