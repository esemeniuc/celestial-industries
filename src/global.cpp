#include "global.hpp"

World world;
std::vector<std::shared_ptr<Entity>> unitMap;
std::vector<std::shared_ptr<Entity>> buildingMap;

std::vector<std::vector<Model::MeshType>> levelArray; //make global so we can have ai know what tiles are around

std::vector<std::vector<AStarNode>> aiCostMap;

std::set<std::shared_ptr<Entity>> unitSeenByAI;
std::set<std::shared_ptr<Entity>> unitOwnedByAI;