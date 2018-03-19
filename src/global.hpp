#pragma once
#include <set>
#include <memory>
#include "world.hpp"

// Global
extern World world;

extern std::vector<std::shared_ptr<Entity>> unitMap;
extern std::vector<std::shared_ptr<Entity>> buildingMap;

extern std::vector<std::vector<Model::MeshType>> levelArray;

extern std::vector<std::vector<AStarNode>> aiCostMap; //ai should be able to see the level traversal costs

extern std::set<std::shared_ptr<Entity>> unitSeenByAI;
extern std::set<std::shared_ptr<Entity>> unitOwnedByAI;