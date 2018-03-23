#include <iostream>
#include "global.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "particle.hpp"

bool Level::init(const std::vector<std::shared_ptr<Renderer>>& meshRenderers) {
	// So that re initializing will be the same as first initialization
	tiles.clear();

	for (size_t i = 0; i < levelArray.size(); i++) {
		std::vector<Model::MeshType> row = levelArray[i];
		std::vector<std::shared_ptr<Tile>> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
			Model::MeshType type = row[j];
			auto renderer = meshRenderers[type];
			std::shared_ptr<Tile> tilePointer;
			switch (type) {
				case Model::MeshType::GUN_TURRET: {
					auto turret = std::make_shared<GunTowerTile>(renderer);
					guntowers.push_back(turret);
					tilePointer = turret;
					break;
				}
				default:
					tilePointer = std::make_shared<Tile>(renderer);
			}
			// TODO: Standardize tile size and resize the model to be the correct size
			tilePointer->translate({j, 0, i});
			tileRow.push_back(tilePointer);
		}
		tiles.push_back(tileRow);
	}
	return true;
}

void Level::update(float ms) {
	for (auto& row : tiles) {
		for (auto& tile : row) {
			tile->update(ms);
		}
	}
}

std::vector<std::vector<Model::MeshType>> Level::levelLoader(const std::string& levelTextFile) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<Model::MeshType>> levelData;
	std::vector<Model::MeshType> row;
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
					row.push_back(Model::MeshType::HROAD);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);
					break;
				}
				case ' ': {
					row.push_back(Model::MeshType::SAND_1);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case '\'': {
					row.push_back(Model::MeshType::SAND_2);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case '.': {
					row.push_back(Model::MeshType::SAND_3);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case ';': {
					row.push_back(Model::MeshType::SAND_4);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case ',': {
					row.push_back(Model::MeshType::SAND_5);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case 'T': {
					row.push_back(Model::MeshType::TREE);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);
					break;
				}
				case 'Y': {
					row.push_back(Model::MeshType::YELLOWTREE);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);
					break;
				}
				case 'R': {
					row.push_back(Model::MeshType::REDTREE);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);
					break;
				}
				case 'W': {
					row.push_back(Model::MeshType::WATER);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);
					break;
				}
				case 'G': {
					row.push_back(Model::MeshType::GRASS);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case 'H': {
					row.push_back(Model::MeshType::HROAD);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case 'V': {
					row.push_back(Model::MeshType::VROAD);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
					break;
				}
				case 'P': {
					row.push_back(Model::MeshType::GEYSER);
					tileData.emplace_back(colNumber, rowNumber, 1000.0, INF);

					Particles::makeParticleEmitter(
							glm::vec3{colNumber, 0, rowNumber}, // emitter position
							glm::vec3{0, 1, 0}, // emitter direction
							1.0,    // spread
							0.1,    // particle width
							0.1,    // particle height
							2.0,    // lifespan
							5.0     // speed
					);
					break;
				}
				default: {
					row.push_back(Model::MeshType::SAND_2);
					tileData.emplace_back(colNumber, rowNumber, 10.0, INF);
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

	std::vector<std::shared_ptr<Tile>> tempRow;
	tempRow.reserve(path.size());
	for (const Coord& component : path) {
		auto tile = std::make_shared<Tile>(Model::meshRenderers[Model::MeshType::SAND_2]);
		tile->translate({component.colCoord, 0, component.rowCoord});
		tempRow.push_back(tile);
	}
	tiles.push_back(tempRow);

	return true;
}