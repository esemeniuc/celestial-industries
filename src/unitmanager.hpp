#pragma once

#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		entityMap.reserve(levelHeight * levelWidth);
	}

	void removeEntity(const std::shared_ptr<Entity> entity) {
		entityMap.erase(std::remove(entityMap.begin(), entityMap.end(), entity), entityMap.end());
	}

	void update(double elapsed_ms) {
		for (auto& entityInACell : entityMap) {
			if (entityInACell->aiComp.currentHealth <= 0) {
				//removeEntity();
			}
			entityInACell->move(elapsed_ms);
			entityInACell->unitComp.update();
		}
	}

}

