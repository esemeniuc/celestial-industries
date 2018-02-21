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

std::vector<std::vector<int>> Level::levelLoader(std::string levelTextFile)
{
	std::ifstream level (levelTextFile);
	std::string line;
	std::vector<std::vector<int>> levelData;	

	if (level.is_open())
	{
		while (getline(level, line))
		{
			std::vector <int> row;
			std::vector<float> costs;
			for (char& tile : line) 
			{
				switch (tile)
				{
					case '#':
					{
						row.push_back(BRICK_CUBE);
						costs.push_back(1000.0);
						break;
					}

					case ' ':
					{
						row.push_back(SAND_1);
						costs.push_back(1.0);
						break;
					}
					default:
					{
						row.push_back(SAND_2);
						costs.push_back(1.0);
						break;
					}
				}
			}
			levelData.push_back(row);
			levelTraversalCostMap.push_back(costs);
		}
		level.close();
	}
	else fprintf(stderr, "Failed to open level data file");
	return levelData;
}

std::vector<std::vector<float>> Level::getLevelTraversalCostMap()
{
	return this->levelTraversalCostMap;
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
