//
// Created by eric on 2/25/18.
//

#include <list>
#include "ai.hpp"

Building* Ai::getHighestValuedBuilding(std::list<Building> &buildings) {
    int highestValueSoFar = -1;
    Building *building;

    if (buildings.size() <= 0) {
        // TODO: take care of case where lenght of vector passed in is 0
    }

    for (auto const& currentBuilding : buildings) {
        if(currentBuilding.buildingValue > highestValueSoFar) {
            highestValueSoFar = currentBuilding.buildingValue;
            building = &currentBuilding;
        }
    }

    return building;
}

