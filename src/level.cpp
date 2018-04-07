#include <iostream>
#include "global.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "particle.hpp"
#include <fstream>

bool Level::init(const std::vector<std::shared_ptr<Renderer>>& meshRenderers) {
	// So that re initializing will be the same as first initialization
	tiles.clear();
	
	for (size_t i = 0; i < Global::levelArray.size(); i++) {
		std::vector<Model::MeshType> row = Global::levelArray[i];
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

// Takes in full path
void Level::save(std::string filename)
{
	std::map<short, char> typeToChar;
	for (const auto& pair : typeToChar) {
		typeToChar[pair.second] = pair.first;
	}
	std::ofstream fs(filename);
	if (!fs) {
		logger(LogLevel::ERR) << "Failed to save level with name " << filename;
		throw "Failed to open file for writing";
	}
	for (const auto& row : Global::levelTraversalCostMap) {
		for (const auto& cell : row) {
			fs << typeToChar[cell.type];
		}
		fs << '\n';
	}
	fs.close();
}

void Level::update(float ms)
{
    for (auto& tile : tiles) {
		tile->update(ms);
    }
}

AStarNode Level::nodeFromCost(int row, int col, Model::MeshType type) {
	std::pair<int, float> cost = tileToCost[type];
	// TODO: Why are we giving this stuff doubles instead of ints?
	return AStarNode(col, row, cost.first, cost.second, (short)type);
}

std::vector<std::vector<Model::MeshType>> Level::levelLoader(
        const std::string& levelTextFile,
        const std::shared_ptr<Shader>& particleShader
) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<Model::MeshType>> levelData;
	std::vector<Model::MeshType> row;
	std::vector<AStarNode> tileData;
	std::shared_ptr<Texture> particleTexture = std::make_shared<Texture>();
	particleTexture->load_from_file(textures_path("turtle.png"));
	if (!particleTexture->is_valid()) {
		throw "Particle texture failed to load!";
	}

	if (!level.is_open()) {
		logger(LogLevel::ERR) << "Failed to open level data file '" << levelTextFile << "'\n";
		return {};
	}

	for (int rowNumber = 0; getline(level, line); rowNumber++) {
		row.clear();
		tileData.clear();
		int colNumber = 0;
		for (const char tile : line) {
			// Handles special stuff to do on special tiles
			switch (tile) {
				case 'P': {
					auto emitter = std::make_shared<Particles::ParticleEmitter>(
                            glm::vec3{colNumber, 0, rowNumber}, // emitter position
                            glm::vec3{0,1,0}, // emitter direction
                            0.8f,    // spread
                            0.5f,    // particle width
                            0.5f,    // particle height
                            2.0f,    // lifespan
                            5.0f,    // speed
                            particleShader,
							particleTexture
                    );
					emitters.push_back(emitter);
                    break;
                }
				default:
					break;
			}

			// Actually add the tiles
			if (charToType.find(tile) == charToType.end()) {
				// Not in map
				row.push_back(Model::MeshType::SAND_2);
				tileData.push_back(nodeFromCost(rowNumber, colNumber, Model::MeshType::SAND_2));
			}
			else {
				row.push_back(charToType[tile]);
				tileData.push_back(nodeFromCost(rowNumber, colNumber, charToType[tile]));
			}
			colNumber++;
		}
		levelData.push_back(row);
		Global::levelTraversalCostMap.push_back(tileData);
	}
	level.close();

	return levelData;
}

std::vector<std::vector<AStarNode>> Level::getLevelTraversalCostMap() {
	return Global::levelTraversalCostMap;
}

std::shared_ptr<Tile> Level::placeTile(Model::MeshType type, glm::vec3 location, unsigned int width, unsigned int height)
{
	for (auto& tile : tiles) {
		if (tile->position.x >= location.x &&
			tile->position.x < location.x + width &&
			tile->position.z >= location.z &&
			tile->position.z < location.z + height) {
			tile->softDelete();
			// TODO: Actually remove the tiles lol (memory is still allocated)
		}
	}

	for (unsigned int x = location.x; x < location.x + width; x++) {
		for (unsigned int y = location.z; y < location.z + height; y++) {
			Global::levelTraversalCostMap[y][x] = nodeFromCost(x,y, type);
		}
	}
	std::shared_ptr<Tile> newTile = tileFromMeshType(type);
	newTile->setPosition(location);
	tiles.push_back(newTile);
	return newTile;
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

void Level::displayPath(const std::vector<Coord>& path) {
	// TODO: Replace tiles in question instead of just adding 2 tiles in one spot
	for (const Coord& component : path) {
		auto tile = std::make_shared<Tile>(Model::MeshType::SAND_2);
		tile->translate({component.colCoord, 0, component.rowCoord});
		tiles.push_back(tile);
	}
}