#include "global.hpp"

namespace Global {
	size_t levelWidth = 0; //gets overwritten in world.init()
	size_t levelHeight = 0;

	size_t windowWidth = Config::INITIAL_WINDOW_WIDTH; //gets overwritten by the resize callback
	size_t windowHeight = Config::INITIAL_WINDOW_HEIGHT;
	
	std::vector<std::shared_ptr<Entity>> playerUnits;
	std::vector<std::shared_ptr<Entity>> aiUnits;

	std::unordered_set<std::shared_ptr<Entity>> playerUnitsSeenByAI;
	std::unordered_set<std::shared_ptr<Entity>> aiUnitsSeenByPlayer;

	std::vector<std::vector<int>> aiVisibilityMap; //stores the last seen time of each cell by ai
	std::vector<std::vector<int>> playerVisibilityMap; //stores the last seen time of each cell by player

	std::vector<std::shared_ptr<Entity>> buildingMap;

	std::vector<std::vector<Model::MeshType>> levelArray; //make global so we can have ai know what tiles are around

	std::vector<std::vector<AStarNode>> aStarCostMap;
}