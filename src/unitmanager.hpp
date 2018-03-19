#pragma once

#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		unitMap.reserve(levelHeight * levelWidth);
	}

	void removeUnit(const std::shared_ptr<Entity>& unit) {
		unitMap.erase(std::remove(unitMap.begin(), unitMap.end(), unit), unitMap.end());
	}

	void update(double elapsed_ms) {
		for (auto& unit : unitMap) {
			unit->move(elapsed_ms);
			unit->unitComp.update();
		}
	}

}

