#pragma once

#include <fstream>
#include "tile.hpp"
#include <map>
#include <tuple>
#include <limits>
#define INF std::numeric_limits<float>::infinity()

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
    GUN_TURRET,
};

// used to build a graph of nodes for the AI pathfinder to traverse
// each tile node.
typedef std::tuple<long /*row*/, long /*col*/, float /*edge weight*/, float /*f-score*/> tileNode;

class Level
{
public:
	bool init(
        std::vector<std::vector<int>> intArray,
        std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources,
        std::shared_ptr<Shader> shader
    );

    void update(float ms);

    // Using a shared pointer to a tile allows us to actually have derived classes in there as well.
	std::vector<std::vector<std::shared_ptr<Tile>>> tiles; // we can add the time dimension when we get there
	std::map<TileType, std::vector<SubObject>> tileTypes;
    std::map<TileType, std::shared_ptr<CompositeObjectBulkRenderer>> tileRenderers;
    std::vector<std::vector<int>> levelLoader(std::string levelTextFile);
    std::vector < std::vector<tileNode>> getLevelTraversalCostMap();
private:
    bool initTileTypes(std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources);
    std::vector<std::vector<tileNode>> levelTraversalCostMap;
};