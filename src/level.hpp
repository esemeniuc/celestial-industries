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
};

class Level
{
public:
	bool init(
        std::vector<std::vector<int>> intArray, 
        std::vector<std::tuple<TileType, std::string>> sources,
        std::shared_ptr<Shader> shader
    );

	std::vector<std::vector<Tile>> tiles; // we can add the time dimension when we get there
	std::vector<BasicEntity> basicEntities;
	std::map<TileType, std::shared_ptr<std::vector<Mesh>>> tileTypes;
    std::map<TileType, std::shared_ptr<OBJBulkRenderer>> tileRenderers;
private:
	bool initTileTypes(std::vector<std::tuple<TileType, std::string>> sources);
};