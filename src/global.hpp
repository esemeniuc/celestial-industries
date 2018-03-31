#pragma once

#include <memory>
#include <unordered_set>
#include <vector>
#include "entity.hpp"
#include "level.hpp"

// Global

namespace Global {
	extern size_t levelWidth;
	extern size_t levelHeight;

	extern size_t windowWidth;
	extern size_t windowHeight;

	extern std::vector<std::shared_ptr<Entity>> playerUnits;
	extern std::vector<std::shared_ptr<Entity>> aiUnits;

	extern std::unordered_set<std::shared_ptr<Entity>> playerUnitsSeenByAI;
	extern std::unordered_set<std::shared_ptr<Entity>> aiUnitsSeenByPlayer;

	extern std::vector<std::vector<int>> aiVisibilityMap; //stores the last seen time of each cell by ai
	extern std::vector<std::vector<int>> playerVisibilityMap; //stores the last seen time of each cell by player

	extern std::vector<std::shared_ptr<Entity>> buildingMap;

	extern std::vector<std::vector<Model::MeshType>> levelArray; //the tiles that make up the level
	extern std::vector<std::vector<AStarNode>> levelTraversalCostMap; //costs for going over each tile

	extern std::unordered_set<Coord, CoordHasher> scoutingTargetsInProgress; //ai uses this to avoid double scouting a position

	extern std::vector<std::vector<AStarNode>> aStarCostMap; //ai should be able to see the level traversal costs

}