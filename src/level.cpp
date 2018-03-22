#include <iostream>
#include "global.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "particle.hpp"

bool Level::init(
    std::vector<std::vector<Model::MeshType>>& levelArray,
    std::vector< std::shared_ptr<Renderer>>& meshRenderers
)
{
    // So that re initializing will be the same as first initialization
	tiles.clear();
	
	for (size_t i = 0; i < levelArray.size(); i++) {
		std::vector<Model::MeshType> row = levelArray[i];
		for (size_t j = 0; j < row.size(); j++) {
            Model::MeshType type = row[j];
			std::shared_ptr<Tile> tilePointer = tileFromMeshType(type);
			// TODO: Standardize tile size and resize the model to be the correct size
            tilePointer->setPosition({ j, 0, i });
            tiles.push_back(tilePointer);
		}
	}
	tileCursor = std::make_shared<Tile>(Model::MeshType::TILE_CURSOR);
	return true;
}

void Level::update(float ms)
{
    for (auto& tile : tiles) {
		tile->update(ms);
    }
}

std::vector<std::vector<Model::MeshType>> Level::levelLoader(const std::string& levelTextFile) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<Model::MeshType>> levelData;
	std::vector<Model::MeshType> row;
	std::vector<AStarNode> tileData;

	tileToCost[Model::MeshType::TREE] = { 1000.0, INF };
	tileToCost[Model::MeshType::SAND_1] = { 10.0, INF };
	tileToCost[Model::MeshType::GUN_TURRET] = { 1000.0, INF };
	tileToCost[Model::MeshType::GEYSER] = { 1000.0, INF };
	tileToCost[Model::MeshType::SAND_2] = { 10.0, INF };

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
					row.push_back(Model::MeshType::TREE);
					tileData.emplace_back(colNumber, rowNumber, tileToCost[Model::MeshType::TREE].first, tileToCost[Model::MeshType::TREE].second);
					break;
				}
				case ' ': {
					row.push_back(Model::MeshType::SAND_1);
					tileData.emplace_back(colNumber, rowNumber, tileToCost[Model::MeshType::SAND_1].first, tileToCost[Model::MeshType::SAND_1].second);
					break;
				}
				case 'G':
					row.push_back(Model::MeshType::GUN_TURRET);
					tileData.emplace_back(colNumber, rowNumber, tileToCost[Model::MeshType::GUN_TURRET].first, tileToCost[Model::MeshType::GUN_TURRET].second);
					break;
				case 'V': {
					row.push_back(Model::MeshType::GEYSER);
					tileData.emplace_back(colNumber, rowNumber, tileToCost[Model::MeshType::GEYSER].first, tileToCost[Model::MeshType::GEYSER].second);

                    Particles::makeParticleEmitter(
                            glm::vec3{colNumber, 0, rowNumber}, // emitter position
                            glm::vec3{0,1,0}, // emitter direction
                            1.0f,    // spread
                            0.1f,    // particle width
                            0.1f,    // particle height
                            2.0f,    // lifespan
                            5.0f     // speed
                    );
                    break;
                }
				default: {
					row.push_back(Model::MeshType::SAND_2);
					tileData.emplace_back(colNumber, rowNumber, tileToCost[Model::MeshType::SAND_2].first, tileToCost[Model::MeshType::SAND_2].second);
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

bool Level::placeTile(Model::MeshType type, glm::vec3 location, unsigned int width, unsigned int height)
{
	Coord levelDimensions = getLevelSize();
	int levelWidth = levelDimensions.rowCoord;
	int levelHeight = levelDimensions.colCoord;
	if (location.x < 0 || location.x + width > levelWidth)
		return false;
	if (location.z < 0 || location.z + height > levelHeight)
		return false;

	for (size_t i = 0; i < tiles.size(); i++) {
		if (
			tiles[i]->position.x >= location.x
			&& tiles[i]->position.x < location.x + width
			&& tiles[i]->position.z >= location.z
			&& tiles[i]->position.z < location.z + height
		) {
			tiles[i]->softDelete();
			// TODO: Actually remove the tiles lol
		}
	}

	for (unsigned int x = location.x; x < location.x + width; x++) {
		for (unsigned int y = location.y; y < location.y + height; y++) {
			levelTraversalCostMap[y][x] = AStarNode(x, y, tileToCost[type].first, tileToCost[type].second);
		}
	}
	std::shared_ptr<Tile> newTile = tileFromMeshType(type);
	newTile->setPosition(location);
	tiles.push_back(newTile);
	return true;
}

bool Level::placeEntity(Model::MeshType type, glm::vec3 location)
{
	std::shared_ptr<Entity> entity = entityFromMeshType(type);
	entity->setPosition(location);
	return true;
}



std::shared_ptr<Tile> Level::tileFromMeshType(Model::MeshType type)
{
	switch (type) {
	case Model::MeshType::GUN_TURRET:
		return std::make_shared<GunTowerTile>();
	default:
		return std::make_shared<Tile>(type);
	}
}

std::shared_ptr<Entity> Level::entityFromMeshType(Model::MeshType type)
{
	switch (type) {
	default:
		return std::make_shared<Entity>(type);
	}
}


bool Level::displayPath(const std::vector<Coord>& path) {
	// TODO: Replace tiles in question instead of just adding 2 tiles in one spot
	for (const Coord& component : path) {
		auto tile = std::make_shared<Tile>(Model::MeshType::SAND_2);
		tile->translate({component.colCoord, 0, component.rowCoord});
		tiles.push_back(tile);
	}

	return true;
}

//returns size in h by w
Coord Level::getLevelSize() const {
	return Coord(levelArray.size(), levelArray.front().size());
}
