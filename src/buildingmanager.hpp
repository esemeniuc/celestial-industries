#pragma once

#include "global.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		Global::buildingMap.reserve(levelHeight * levelWidth);
	}

	void update(double elapsed_ms) {
		
	}

}
