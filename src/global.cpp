#include "global.hpp"

namespace Global {
	size_t levelWidth = 0; //gets overwritten in world.init()
	size_t levelHeight = 0;

	double playerResources = 400;
	double playerResourcesPerSec = 0;
	int playerCurrentSupply = 0;
	int playerMaxSupply = 10;

	GameState gameState = GameState::START_MENU;

	size_t windowWidth = Config::INITIAL_WINDOW_WIDTH; //gets overwritten by the resize callback
	size_t windowHeight = Config::INITIAL_WINDOW_HEIGHT;

	std::vector<std::shared_ptr<Entity>> selectedEntities; //highlighted by the mouse
	std::shared_ptr<Tile> selectedBuilding; // highlighted by mouse; should be mutually exclusive with selectedEntities

	std::vector<std::shared_ptr<Entity>> playerUnits;
	std::vector<std::shared_ptr<Entity>> aiUnits;

	std::unordered_set<std::shared_ptr<Entity>> playerUnitsSeenByAI;
	std::unordered_set<std::shared_ptr<Entity>> aiUnitsSeenByPlayer;

	std::vector<std::vector<int>> aiVisibilityMap; //stores the last seen time of each cell by ai
	std::vector<std::vector<int>> playerVisibilityMap; //stores the last seen time of each cell by player

	std::vector<std::shared_ptr<Entity>> buildingList;
	std::vector<std::shared_ptr<Tile>> buildingTileList;

	std::vector<std::vector<Model::MeshType>> levelArray; //make global so we can have ai know what tiles are around
	std::vector<std::vector<int>> levelTraversalCostMap;
	std::vector<std::vector<int>> levelWithUnitsTraversalCostMap;

	std::unordered_set<Coord, CoordHasher> scoutingTargetsInProgress;

	std::vector<std::shared_ptr<Particles::ParticleEmitter>> emitters;

	std::vector<std::shared_ptr<Weapon>> weapons;
}
