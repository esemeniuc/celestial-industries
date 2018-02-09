#include "level.hpp"
#include <iostream>
#include <map>

bool Level::init(std::vector<std::vector<int>> intArray, std::vector<std::tuple<TileType, std::string>> sources)
{
	if (!initTileTypes(sources)) {
		std::cout << "Failed to init tile types!" << std::endl;
		return false;
	}
	
	// So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < intArray.size(); i++) {
		std::vector<int> row = intArray[i];
		std::vector<Tile> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
			int cell = row[j];
			Tile tile;
			bool success = tile.init(tileTypes[static_cast<TileType>(cell)]);
			if (!success) {
				std::cout << "FAILED TO INITIALIZE TILE OF TYPE " << cell << std::endl;
			}
			// TODO: Standardize tile size and resize the model to be the correct size
			tile.translate({ j, 0, i });
			tileRow.push_back(tile);
		}
		tiles.push_back(tileRow);
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
		tileTypes[tileType] = obj;
	}
	return true;
}
