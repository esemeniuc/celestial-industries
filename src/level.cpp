#include "level.hpp"
#include <iostream>
#include "logger.hpp"

bool Level::init(
    std::vector<std::vector<Config::MeshType>> levelArray,
    std::map<Config::MeshType, std::shared_ptr<Renderer>> meshRenderers
)
{
    // So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < levelArray.size(); i++) {
		std::vector<Config::MeshType> row = levelArray[i];
		std::vector<std::shared_ptr<Tile>> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
            Config::MeshType type = row[j];
            auto renderer = meshRenderers[type];
            std::shared_ptr<Tile> tilePointer;
            switch (type) {
            case Config::MeshType::GUN_TURRET: {
                auto turret = std::make_shared<GunTowerTile>(renderer);
                guntowers.push_back(turret);
                tilePointer = turret;
                break;
            }
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

std::vector<std::vector<Config::MeshType>> Level::levelLoader(const std::string& levelTextFile) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<Config::MeshType>> levelData;
	std::vector<Config::MeshType> row;
	std::vector<AStarNode> tileData;

	if (!level.is_open()) {
		logger(LogLevel::ERR) << "Failed to open level data file '" << levelTextFile << "'\n";
		return {};
	}

	for (int rowNumber = 0; getline(level, line); rowNumber++) {
		row.clear();
		tileData.clear();
		int colNumber = 0;
		for (const char tile : line) {
			switch (tile) {
				case '#': {
					row.push_back(Config::MeshType::TREE);
					tileData.emplace_back(rowNumber, colNumber, 1000.0, INF);
					break;
				}
				case ' ': {
					row.push_back(Config::MeshType::SAND_1);
					tileData.emplace_back(rowNumber, colNumber, 10.0, INF);
					break;
				}
                case 'G':
                    row.push_back(Config::MeshType::GUN_TURRET);
                    tileData.emplace_back(rowNumber, colNumber, 1000.0, INF);
                    break;
				default: {
					row.push_back(Config::MeshType::SAND_2);
					tileData.emplace_back(rowNumber, colNumber, 10.0, INF);
					break;
				}
			}
			colNumber++;
		}
		levelData.push_back(row);
		levelTraversalCostMap.push_back(tileData);
	}
	level.close();

	return levelData;
}

std::vector<std::vector<AStarNode>> Level::getLevelTraversalCostMap() {
	return this->levelTraversalCostMap;
}


bool Level::displayPath(const std::vector<Coord>& path) {

//	for (const Coord& component : path) {
//		std::vector<std::shared_ptr<Tile>> tileRow;
//        auto renderer = meshRenderers[TileType::SAND_2];
//        std::shared_ptr<Tile> tile = std::make_shared<Tile>(renderer);
//
//		tile->translate({component.colCoord, 0, component.rowCoord});
//		tileRow.push_back(tile);
//		tiles.push_back(tileRow);
//	}

	return true;
}
