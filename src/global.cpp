#include "global.hpp"

World world;
std::vector<std::vector<std::vector<Entity>>> entityMap; //2d map of entities, where more than 1 entity can be in a

std::vector<std::vector<Model::MeshType>> levelArray; //make global so we can have ai know what tiles are around

std::vector<std::vector<AStarNode>> aiCostMap;