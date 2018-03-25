#pragma once

#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		playerUnits.reserve(levelHeight * levelWidth);
		aiUnits.reserve(levelHeight * levelWidth);
	}

	void update(double elapsed_ms) {
		for (auto& playerUnit : playerUnits) {
				playerUnit->unitComp.update();
				playerUnit->move(elapsed_ms);
		}

		for (auto& aiUnit : aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
		}
	}

}

