#pragma once
#include <memory>
#include "world.hpp"

// Global
extern World world;

extern std::vector<std::shared_ptr<Entity>> entityMap;

extern std::vector<std::vector<Model::MeshType>> levelArray;

extern std::vector<std::vector<AStarNode>> aiCostMap; //ai should be able to see the level traversal costs
