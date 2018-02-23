#include "level.hpp"
#include <iostream>
#include <map>

bool Level::init(
    std::vector<std::vector<int>> intArray,
    std::vector<std::tuple<TileType, std::string>> sources,
    std::shared_ptr<Shader> shader
)
{
    // Initialize the tile type -> obj map
	if (!initTileTypes(sources)) {
        logger(LogLevel::ERR) << "Failed to init tile types!" << Logger::endl;
		return false;
	}
	
    // Initialize the tile type -> bulk renderer map
    for (auto tileType : tileTypes) {
        // TODO: I think this obsoletes the previous map, but maybe not?
        tileRenderers[tileType.first] = std::make_shared<OBJBulkRenderer>(OBJBulkRenderer(shader, tileType.second));
    }

	// So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < intArray.size(); i++) {
		std::vector<int> row = intArray[i];
		std::vector<Tile> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
			int cell = row[j];
            TileType type = static_cast<TileType>(cell);
            auto renderer = tileRenderers[type];
			Tile tile = Tile(renderer);
			// TODO: Standardize tile size and resize the model to be the correct size
			tile.translate({ j, 0, i });
		}
	}
	return true;
}

bool Level::initTileTypes(std::vector<std::tuple<TileType, std::string>> sources)
{
	// All the models come from the same place
    std::string path = pathBuilder({ "data", "models" });
	for (auto source : sources) {
		TileType tileType = std::get<0>(source);
		std::string filename = std::get<1>(source);
		OBJ::Data obj;
		if (!OBJ::Loader::loadOBJ(path, filename, obj)) {
			// Failure message should already be handled by loadOBJ
			return false;
		}
        auto meshResult = objToMesh(obj);
        if (!meshResult.first) {
            logger(LogLevel::ERR) << "Failed to turn tile obj to mesh for tile " << filename  << Logger::endl;
        }
		tileTypes[tileType] = meshResult.second;
	}
	return true;
}
