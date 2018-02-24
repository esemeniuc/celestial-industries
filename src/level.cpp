#include "level.hpp"
#include <iostream>
#include <map>

bool Level::init(
    std::vector<std::vector<int>> intArray,
    std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources,
    std::shared_ptr<Shader> shader
)
{
    // Initialize the tile type -> obj map
    if (!initTileTypes(sources)) {
        logger(LogLevel::ERR) << "Failed to init complex tile types!" << Logger::endl;
        return false;
    }
	
    // Initialize the tile type -> bulk renderer map
    for (auto tileType : tileTypes) {
        // TODO: I think this obsoletes the previous map, but maybe not?
        tileRenderers[tileType.first] = std::make_shared<CompositeObjectBulkRenderer>(shader, tileType.second);
    }

	// So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < intArray.size(); i++) {
		std::vector<int> row = intArray[i];
		std::vector<std::shared_ptr<Tile>> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
			int cell = row[j];
            TileType type = static_cast<TileType>(cell);
            auto renderer = tileRenderers[type];
            std::shared_ptr<Tile> tilePointer;
            switch (type) {
            case GUN_TURRET:
                tilePointer = std::make_shared<GunTowerTile>(renderer);
                break;
            default:
                tilePointer = std::make_shared<Tile>(renderer);
            }
			// TODO: Standardize tile size and resize the model to be the correct size
            tilePointer->translate({ j, 0, i });
            tileRow.push_back(tilePointer);
		}
        tiles.push_back(tileRow);
	}
	return true;
}

void Level::update(float ms)
{
    for (auto& row : tiles) {
        for (auto& tile : row) {
            tile->update(ms);
        }
    }
}

bool Level::initTileTypes(std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources)
{
    // All the models come from the same place
    std::string path = pathBuilder({ "data", "models" });
    for (auto source : sources) {

        std::vector<SubObject> subObjects;
        TileType tileType = source.first;
        std::vector<SubObjectSource> objSources  = source.second;
        for (auto objSource : objSources) {
            OBJ::Data obj;
            if (!OBJ::Loader::loadOBJ(path, objSource.filename, obj)) {
                // Failure message should already be handled by loadOBJ
                return false;
            }
            auto meshResult = objToMesh(obj);
            if (!meshResult.first) {
                logger(LogLevel::ERR) << "Failed to turn tile obj to meshes for tile " << objSource.filename << Logger::endl;
            }
            subObjects.push_back({
                meshResult.second,
                objSource.parentMesh
            });
        }
        tileTypes[tileType] = subObjects;
    }
    return true;
}

