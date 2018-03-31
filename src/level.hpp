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
	short type;
	int rowCoord, colCoord, movementCost;
	float fScore;

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
	std::vector<std::shared_ptr<Particles::ParticleEmitter>> emitters;

	//funcs
	bool init(const std::vector<std::shared_ptr<Renderer>>& meshRenderers);

	void save(std::string filename);

	void update(float ms);

	bool displayPath(const std::vector<Coord>& levelArray);

	std::vector<std::vector<Model::MeshType>> levelLoader(
			const std::string& levelTextFile,
			const std::shared_ptr<Shader>& particleShader
	);

	std::vector<std::vector<AStarNode>> getLevelTraversalCostMap();

	AStarNode nodeFromCost(int row, int col, Model::MeshType type);

	// Places a tile, replaceing anything there before. If the tile is larger than standard specify the width and height.
	// The location reffer's to the tile's top left corner (0,0,0) being the minimum accepted. The location is NOT the center of the tile.
	std::shared_ptr<Tile>
	placeTile(Model::MeshType type, glm::vec3 location, unsigned int width = 1, unsigned int height = 1);

	std::shared_ptr<Entity> placeEntity(Model::MeshType type, glm::vec3 location, GamePieceOwner owner);

	std::shared_ptr<Tile> tileFromMeshType(Model::MeshType type);

	std::shared_ptr<Entity> entityFromMeshType(Model::MeshType type);

	// Needed to properly update cost map when placing tiles

	// Indexable using MeshType enum
	std::map<Model::MeshType, std::pair<int, float>> tileToCost{
			{Model::MeshType::HROAD,      {Config::OBSTACLE_COST,            INF}},
			{Model::MeshType::SAND_1,     {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::SAND_2,     {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::SAND_3,     {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::SAND_4,     {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::SAND_5,     {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::TREE,       {Config::OBSTACLE_COST,            INF}},
			{Model::MeshType::YELLOWTREE, {Config::OBSTACLE_COST,            INF}},
			{Model::MeshType::REDTREE,    {Config::OBSTACLE_COST,            INF}},
			{Model::MeshType::WATER,      {Config::OBSTACLE_COST,            INF}},
			{Model::MeshType::GRASS,      {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::HROAD,      {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::VROAD,      {Config::DEFAULT_TRAVERSABLE_COST, INF}},
			{Model::MeshType::GEYSER,     {Config::OBSTACLE_COST,            INF}}
	};

	std::map<char, Model::MeshType> charToType{
			{'#',  Model::MeshType::HROAD},
			{' ',  Model::MeshType::SAND_1},
			{'\\', Model::MeshType::SAND_2},
			{'.',  Model::MeshType::SAND_3},
			{';',  Model::MeshType::SAND_4},
			{',',  Model::MeshType::SAND_5},
			{'T',  Model::MeshType::TREE},
			{'Y',  Model::MeshType::YELLOWTREE},
			{'R',  Model::MeshType::REDTREE},
			{'W',  Model::MeshType::WATER},
			{'G',  Model::MeshType::GRASS},
			{'H',  Model::MeshType::HROAD},
			{'V',  Model::MeshType::VROAD},
			{'P',  Model::MeshType::GEYSER},
			{'X',  Model::MeshType::GUN_TURRET}
	};

private:
	//members

	//funcs
};
