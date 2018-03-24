#pragma once

#include "global.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		buildingMap.reserve(levelHeight * levelWidth);
	}

	bool isDead( std::shared_ptr<Entity>& unit)
	{
		if(unit->aiComp.currentHealth <= 0)
		{
			unit.get()->softDelete();
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead() {
		// std::cout << "buildings before: " << building.size() << '\n';
		buildingMap.erase(std::remove_if(buildingMap.begin(), buildingMap.end(), isDead), buildingMap.end());
		// std::cout << "buildings after: " << playerUnits.size() << '\n';
	}

	void update(double elapsed_ms) {
		removeDead();
		for (auto& building : buildingMap) {
			if (building.get()->aiComp.currentHealth <= 0) {
				building.get()->softDelete();
			}
		}
	}

}