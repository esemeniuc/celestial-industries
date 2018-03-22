#pragma once

#include "global.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		buildingMap.reserve(levelHeight * levelWidth);
	}

	bool isDead( std::shared_ptr<Entity>& unit) //THIS IS A TERRIBLE HACK, FIXME
	{
		if(unit->aiComp.currentHealth <= 0)
		{
			unit->translate({999,999,999});
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead(std::shared_ptr<Entity> building) {
		std::cout << "buildings before: " << building.size() << '\n';
		buildingMap.erase(std::remove_if(buildingMap.begin(), buildingMap.end(), isDead), buildingMap.end());
		std::cout << "buildings after: " << playerUnits.size() << '\n';
	}

	void update(double elapsed_ms) {
		removeDead();
		for (auto& building : buildingMap) {
			if (building.get()->aiComp.currentHealth <= 0) {
				removeBuilding(building);
			}
		}
	}

}