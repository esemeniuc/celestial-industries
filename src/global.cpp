#include "global.hpp"

World world;
std::vector<std::shared_ptr<Entity>> playerUnits;
std::vector<std::shared_ptr<Entity>> aiUnits;

std::unordered_set<std::shared_ptr<Entity>> playerUnitsSeenByAI;
std::unordered_set<std::shared_ptr<Entity>> aiUnitsSeenByPlayer;

std::vector<std::shared_ptr<Entity>> buildingMap;

std::vector<std::vector<Model::MeshType>> levelArray; //make global so we can have ai know what tiles are around

std::vector<std::vector<AStarNode>> aiCostMap;
