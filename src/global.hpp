#pragma once

#include "world.hpp"

// Global
extern World world;

extern std::vector<std::vector<std::vector<Entity>>> entityMap; //2d map of entities, where more than 1 entity can be in a

extern std::vector<std::vector<Model::MeshType>> levelArray;

extern std::vector<std::vector<AStarNode>> aiCostMap; //ai should be able to see the level traversal costs
