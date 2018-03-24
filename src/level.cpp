#include <iostream>
#include "global.hpp"
#include "level.hpp"
#include "logger.hpp"
#include "particle.hpp"
#include <fstream>

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

// Takes in full path
void Level::save(std::string filename)
{
	std::map<short, char> typeToChar;
	for (std::map<char, Model::MeshType>::iterator iter = charToType.begin(); iter != charToType.end(); ++iter) {
		typeToChar[iter->second] = iter->first;
	}
	std::ofstream fs(filename);
	if (!fs) {
		logger(LogLevel::ERR) << "Failed to save level with name " << filename;
		throw "Failed to open file for writing";
	}
	for (const auto& row : levelTraversalCostMap) {
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

	// Needed to properly update cost map when placeing tiles 
	tileToCost = {
		{Model::MeshType::HROAD, { 1000, INF }},
		{Model::MeshType::SAND_1, { 10, INF }},
		{Model::MeshType::SAND_2, { 10, INF }},
		{Model::MeshType::SAND_3, { 10, INF }},
		{Model::MeshType::SAND_4, { 10, INF }},
		{Model::MeshType::SAND_5, { 10, INF }},
		{Model::MeshType::TREE, {1000, INF}},
		{Model::MeshType::REDTREE, {1000, INF}},
		{Model::MeshType::WATER, {1000, INF}},
		{Model::MeshType::GRASS, {10, INF}},
		{Model::MeshType::HROAD, {10, INF}},
		{Model::MeshType::VROAD, {10, INF}},
		{Model::MeshType::GEYSER, {1000, INF}}
	};

	charToType = {
		{'#', Model::MeshType::HROAD},
		{' ', Model::MeshType::SAND_1},
		{'\\', Model::MeshType::SAND_2},
		{'.', Model::MeshType::SAND_3},
		{';', Model::MeshType::SAND_4},
		{',', Model::MeshType::SAND_5},
		{'T', Model::MeshType::TREE},
		{'Y', Model::MeshType::YELLOWTREE},
		{'R', Model::MeshType::REDTREE},
		{'W', Model::MeshType::WATER},
		{'G', Model::MeshType::GRASS},
		{'H', Model::MeshType::HROAD},
		{'V', Model::MeshType::VROAD},
		{'P', Model::MeshType::GEYSER},
		{'X', Model::MeshType::GUN_TURRET}
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
	Coord levelDimensions = getLevelSize();
	int levelWidth = levelDimensions.rowCoord;
	int levelHeight = levelDimensions.colCoord;

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
			levelTraversalCostMap[y][x] = nodeFromCost(x,y, type);
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

//returns size in h by w
Coord Level::getLevelSize() const {
	return Coord(levelArray.size(), levelArray.front().size());
}
