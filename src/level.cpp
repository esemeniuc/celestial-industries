#include "level.hpp"
#include <iostream>
#include "logger.hpp"

bool Level::init(
    std::vector<std::vector<TileType>> levelArray,
    std::vector<std::pair<TileType, std::vector<SubObjectSource>>> sources,
    std::shared_ptr<Shader> shader
)
{
    // Initialize the tile type -> obj map
    if (!initTileTypes(sources)) {
        logger(LogLevel::ERR) << "Failed to init complex tile types!" << '\n';
        return false;
    }
	
    // Initialize the tile type -> bulk renderer map
    for (auto tileType : tileTypes) {
        // TODO: I think this obsoletes the previous map, but maybe not?
        tileRenderers[tileType.first] = std::make_shared<CompositeObjectBulkRenderer>(shader, tileType.second);
    }

	// So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < levelArray.size(); i++) {
		std::vector<TileType> row = levelArray[i];
		std::vector<std::shared_ptr<Tile>> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
            TileType type = row[j];
            auto renderer = tileRenderers[type];
            std::shared_ptr<Tile> tilePointer;
            switch (type) {
            case TileType::GUN_TURRET:
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

std::vector<std::vector<TileType>> Level::levelLoader(const std::string& levelTextFile) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<TileType>> levelData;
	std::vector<TileType> row;
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
					row.push_back(TileType::TREE);
					tileData.emplace_back(rowNumber, colNumber, 1000.0, INF);
					break;
				}
				case ' ': {
					row.push_back(TileType::SAND_1);
					tileData.emplace_back(rowNumber, colNumber, 10.0, INF);
					break;
				}
				default: {
					row.push_back(TileType::SAND_2);
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
                logger(LogLevel::ERR) << "Failed to turn tile obj to meshes for tile " << objSource.filename << '\n';
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


bool Level::displayPath(const std::vector<Coord>& path) {

	for (const Coord& component : path) {
		std::vector<std::shared_ptr<Tile>> tileRow;
        auto renderer = tileRenderers[TileType::SAND_2];
        std::shared_ptr<Tile> tile = std::make_shared<Tile>(renderer);

		tile->translate({component.colCoord, 0, component.rowCoord});
		tileRow.push_back(tile);
		tiles.push_back(tileRow);
	}

	return true;
}
