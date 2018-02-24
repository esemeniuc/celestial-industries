#pragma once

#include "tile.hpp"
#include "basicentity.hpp"
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
    GUN_TURRET,
};

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
	std::vector<BasicEntity> basicEntities;
	std::map<TileType, std::vector<SubObject>> tileTypes;
    std::map<TileType, std::shared_ptr<CompositeObjectBulkRenderer>> tileRenderers;
private:
    bool initTileTypes(std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources);
};