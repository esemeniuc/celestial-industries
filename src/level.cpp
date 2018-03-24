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

AStarNode nodeFromCost(int row, int col, std::pair<double, float> cost) {
	// TODO: Why are we giving this stuff doubles instead of ints?
	return AStarNode(col, row, cost.first, cost.second);
}

std::vector<std::vector<Model::MeshType>> Level::levelLoader(const std::string& levelTextFile) {
	std::ifstream level(levelTextFile);
	std::string line;
	std::vector<std::vector<Model::MeshType>> levelData;
	std::vector<Model::MeshType> row;
	std::vector<AStarNode> tileData;

	// Needed to properly update cost map when placeing tiles 
	tileToCost = {
		{Model::MeshType::HROAD, { 1000.0, INF }},
		{Model::MeshType::SAND_1, { 10.0, INF }},
		{Model::MeshType::SAND_2, { 10.0, INF }},
		{Model::MeshType::SAND_3, { 10.0, INF }},
		{Model::MeshType::SAND_4, { 10.0, INF }},
		{Model::MeshType::SAND_5, { 10.0, INF }},
		{Model::MeshType::TREE, {1000.0, INF}},
		{Model::MeshType::REDTREE, {1000.0, INF}},
		{Model::MeshType::WATER, {1000.0, INF}},
		{Model::MeshType::GRASS, {10.0, INF}},
		{Model::MeshType::HROAD, {10.0, INF}},
		{Model::MeshType::VROAD, {10.0, INF}},
		{Model::MeshType::GEYSER, {1000.0, INF}}
	};

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
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::HROAD]));
					break;
				}
				case ' ': {
					row.push_back(Model::MeshType::SAND_1);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_1]));
					break;
				}
				case '\'': {
					row.push_back(Model::MeshType::SAND_2);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_2]));
					break;
				}
				case '.': {
					row.push_back(Model::MeshType::SAND_3);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_3]));
					break;
				}
				case ';': {
					row.push_back(Model::MeshType::SAND_4);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_4]));
					break;
				}
				case ',': {
					row.push_back(Model::MeshType::SAND_5);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_5]));
					break;
				}
				case 'T': {
					row.push_back(Model::MeshType::TREE);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::TREE]));
					break;
				}
				case 'Y': {
					row.push_back(Model::MeshType::YELLOWTREE);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::YELLOWTREE]));
					break;
				}
				case 'R': {
					row.push_back(Model::MeshType::REDTREE);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::REDTREE]));
					break;
				}
				case 'W': {
					row.push_back(Model::MeshType::WATER);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::WATER]));
					break;
				}
				case 'G': {
					row.push_back(Model::MeshType::GRASS);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::GRASS]));
					break;
				}
				case 'H': {
					row.push_back(Model::MeshType::HROAD);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::HROAD]));
					break;
				}
				case 'V': {
					row.push_back(Model::MeshType::VROAD);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::VROAD]));
					break;
				}
				case 'P': {
					row.push_back(Model::MeshType::GEYSER);
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::GEYSER]));

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
					tileData.push_back(nodeFromCost(rowNumber, colNumber, tileToCost[Model::MeshType::SAND_2]));
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

std::shared_ptr<Tile> Level::placeTile(Model::MeshType type, glm::vec3 location, unsigned int width, unsigned int height)
{
	for (size_t i = 0; i < tiles.size(); i++) {
		if (
			tiles[i]->position.x >= location.x
			&& tiles[i]->position.x < location.x + width
			&& tiles[i]->position.z >= location.z
			&& tiles[i]->position.z < location.z + height
		) {
			tiles[i]->softDelete();
			// TODO: Actually remove the tiles lol (memory is still allocated)
		}
	}

	for (unsigned int x = location.x; x < location.x + width; x++) {
		for (unsigned int y = location.z; y < location.z + height; y++) {
			levelTraversalCostMap[y][x] = AStarNode(x, y, tileToCost[type].first, tileToCost[type].second);
		}
	}
	std::shared_ptr<Tile> newTile = tileFromMeshType(type);
	newTile->setPosition(location);
	tiles.push_back(newTile);
	return newTile;
}

// Sets AI comp and Unit comp
void initUnitFromMeshType(std::shared_ptr<Entity> e, Model::MeshType type, GamePieceOwner owner) {
	switch (type) {
	case Model::MeshType::FRIENDLY_FIRE_UNIT:
	case Model::MeshType::ENEMY_SPIKE_UNIT:
		e->aiComp.initialHealth = 160;
		e->aiComp.visionRange = 8;
		e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
		e->aiComp.currentHealth = e->aiComp.initialHealth;
		e->aiComp.value = 100;

		e->unitComp.initialEnergyLevel = 50;
		e->unitComp.attackDamage = 10;
		e->unitComp.attackRange = 1;
		e->unitComp.attackSpeed = 5;
		e->unitComp.movementSpeed = 3;
		e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
		e->unitComp.state = UnitState::IDLE;
		break;
	case Model::MeshType::FRIENDLY_RANGED_UNIT:
	case Model::MeshType::ENEMY_RANGED_RADIUS_UNIT:
	case Model::MeshType::ENEMY_RANGED_LINE_UNIT:
		e->aiComp.initialHealth = 45;
		e->aiComp.visionRange = 8;
		e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
		e->aiComp.currentHealth = e->aiComp.initialHealth;
		e->aiComp.value = 50;

		e->unitComp.initialEnergyLevel = 50;
		e->unitComp.attackDamage = 6;
		e->unitComp.attackRange = 5;
		e->unitComp.attackSpeed = 5;
		e->unitComp.movementSpeed = 3;
		e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
		e->unitComp.state = UnitState::IDLE;
		break;
	default:
		throw "Un initializeable unit encountered in initUnitFromMeshType";
	}

	e->aiComp.owner = owner;

	if (owner == GamePieceOwner::PLAYER) {
		playerUnits.push_back(e);

	}
	else if (owner == GamePieceOwner::AI) {
		aiUnits.push_back(e);
	}
}

std::shared_ptr<Entity> Level::placeEntity(Model::MeshType type, glm::vec3 location, GamePieceOwner owner)
{
	std::shared_ptr<Entity> entity = entityFromMeshType(type);
	entity->setPosition(location);
	initUnitFromMeshType(entity, type, owner);
	return entity;
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
	case Model::MeshType::FRIENDLY_RANGED_UNIT:
		return std::make_shared<TurretUnit>(type, 2);
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