#pragma once

#include <limits>
#include <ostream> //for overloaded << operator
#include <map>
#include "common.hpp"
#include "config.hpp"
#include "tile.hpp"
#include "model.hpp"
#include "particle.hpp"
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
	short type;

	AStarNode() = default;

	AStarNode(int _colCoord, int _rowCoord, int _movementCost, float _fScore, short _type) : rowCoord(_rowCoord),
																							 colCoord(_colCoord),
																							 movementCost(
																									 _movementCost),
																							 fScore(_fScore),
																							 type(_type) {}

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
	std::shared_ptr<Tile> tileCursor;

	std::shared_ptr<Shader> particleShader;
	std::shared_ptr<Texture> particleTexture;

	//funcs
	bool init(const std::vector<std::shared_ptr<Renderer>>& meshRenderers);

	void save(std::string filename);

	void update(float ms);

	void displayPath(const std::vector<Coord>& levelArray);

	std::vector<std::vector<Model::MeshType>> levelLoader(const std::string& levelTextFile);

	std::vector<std::vector<AStarNode>> getLevelTraversalCostMap();

	AStarNode nodeFromCost(int row, int col, Model::MeshType type);

	// Places a tile, replacing anything there before. If the tile is larger than standard specify the width and height.
	// The location refers to the tile's top left corner (0,0,0) being the minimum accepted. The location is NOT the center of the tile.
	std::shared_ptr<Tile> placeTile(Model::MeshType type, glm::vec3 location, GamePieceOwner owner = GamePieceOwner::PLAYER, unsigned int width = 1, unsigned int height = 1, int extraArg = 0, Model::MeshType replacingMesh = Model::MeshType::SAND_1);

	int numTilesOfTypeInArea(Model::MeshType type, glm::vec3 location, unsigned int height = 1, unsigned int width = 1);

	int numTilesOfOwnerInArea(GamePieceOwner owner, glm::vec3 location, unsigned int height = 1, unsigned int width = 1);

	std::shared_ptr<Tile> tileFromMeshType(Model::MeshType type, int extraArg = 0);

	void setupAiCompForTile(std::shared_ptr<Tile> tile, GamePieceOwner owner);

	// Indexable using MeshType enum
	static std::map<Model::MeshType, std::pair<int, float>> tileToCost;

	static std::map<char, Model::MeshType> charToType;

private:
	//members

	//funcs
};
