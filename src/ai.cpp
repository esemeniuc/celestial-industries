//
// Created by eric on 2/25/18.
//

#include <list>
#include "ai.hpp"

Building* Ai::bestBuildingToAttack(std::list<Building>& buildings, Entity& entity) {
    float bestAttackValue = -1;
    Building *building;

    if (buildings.size() <= 0) {
        // TODO: take care of case where length of building list passed in is 0
    }

    for (auto& currentBuilding : buildings) {
        int buildingValue = currentBuilding.buildingValue;
        float distanceToBuilding = getDistanceBetweenEntities(currentBuilding, entity);
        float attackValue = buildingValue - (distanceToBuilding * PRIORITIZE_CLOSER_ATTACKS);

        if (attackValue > bestAttackValue) {
            bestAttackValue = attackValue;
            building = &currentBuilding;
        }
    }

    return building;
}

Building* Ai::getHighestValuedBuilding(std::list<Building>& buildings) {
    int highestValueSoFar = -1;
    Building *building;

    if (buildings.size() <= 0) {
        // TODO: take care of case where length of building list passed in is 0
    }

    for (auto& currentBuilding : buildings) {
        if(currentBuilding.buildingValue > highestValueSoFar) {
            highestValueSoFar = currentBuilding.buildingValue;
            building = &currentBuilding;
        }
    }

    return building;
}

float Ai::getDistanceBetweenEntities(Entity &entity1, Entity &entity2) {
    glm::vec3 p1 = entity1.getPosition();
    glm::vec3 p2 = entity2.getPosition();

    // Ignore z coordinate.
    float diffY = p1.y - p2.y;
    float diffX = p1.x - p2.x;
    return sqrt((diffY * diffY) + (diffX * diffX));
}
