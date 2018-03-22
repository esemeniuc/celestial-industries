#pragma once

#include "global.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		buildingMap.reserve(levelHeight * levelWidth);
	}

	void removeBuilding(const std::shared_ptr<Entity>& building) {
		buildingMap.erase(std::remove(buildingMap.begin(), buildingMap.end(), building), buildingMap.end());
	}

	void update(double elapsed_ms) {
		for (auto& building : buildingMap) {
			if (building.get()->aiComp.currentHealth <= 0) {
				removeBuilding(building);
			}
		}
	}

}