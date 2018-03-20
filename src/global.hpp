#pragma once

#include <memory>
#include <unordered_set>
#include "world.hpp"

// Global
extern World world;

extern std::vector<std::shared_ptr<Entity>> playerUnits;
extern std::vector<std::shared_ptr<Entity>> aiUnits;

extern std::unordered_set<std::shared_ptr<Entity>> playerUnitsSeenByAI;
extern std::unordered_set<std::shared_ptr<Entity>> aiUnitsSeenByPlayer;

extern std::vector<std::shared_ptr<Entity>> buildingMap;

extern std::vector<std::vector<Model::MeshType>> levelArray;

extern std::vector<std::vector<AStarNode>> aiCostMap; //ai should be able to see the level traversal costs

