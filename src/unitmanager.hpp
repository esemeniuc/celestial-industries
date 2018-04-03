#pragma once

#include <vector>
#include "global.hpp"
#include "aimanager.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		Global::aiVisibilityMap = std::vector<std::vector<int>>(levelHeight, std::vector<int>(levelWidth));
		Global::playerVisibilityMap = std::vector<std::vector<int>>(levelHeight, std::vector<int>(levelWidth));
	}

	bool isDead(std::shared_ptr<Entity>& unit) {
		if (unit->aiComp.currentHealth <= 0) {
			unit->softDelete();
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead() {
		//std::cout << "before: " << playerUnits.size() << '\n';
		Global::playerUnits.erase(std::remove_if(Global::playerUnits.begin(), Global::playerUnits.end(), isDead),
								  Global::playerUnits.end());
		//std::cout << "after: " << playerUnits.size() << '\n';
		Global::aiUnits.erase(std::remove_if(Global::aiUnits.begin(), Global::aiUnits.end(), isDead),
							  Global::aiUnits.end());
	}

	void updateAreaSeenByUnit(const std::shared_ptr<Entity>& unit, int currentUnixTime,
							  std::vector<std::vector<int>>& visibilityMap) {
		int radius = unit->aiComp.visionRange;
		const int xCenter = unit->getPositionInt().colCoord;
		const int yCenter = unit->getPositionInt().rowCoord;
		int xMin = std::max(0, xCenter - radius);
		int xMax = std::min((int) Global::levelWidth, xCenter + radius);
		int yMin = std::max(0, yCenter - radius);
		int yMax = std::min((int) Global::levelHeight, yCenter + radius);

		for (int i = yMin; i < yMax; ++i) {
			for (int j = xMin; j < xMax; ++j) {
				int yp = i - yCenter;
				int xp = j - xCenter;
				if (xp * xp + yp * yp <= radius * radius) {
					visibilityMap[i][j] = currentUnixTime;
				}
			}
		}
	}

	void update(double elapsed_ms) {
		removeDead();
		int currentUnixTime = (int) getUnixTime();
		for (auto& playerUnit : Global::playerUnits) {
			playerUnit->unitComp.update();
			playerUnit->move(elapsed_ms);
			updateAreaSeenByUnit(playerUnit, currentUnixTime, Global::playerVisibilityMap);
		}

		for (auto& aiUnit : Global::aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
			updateAreaSeenByUnit(aiUnit, currentUnixTime, Global::aiVisibilityMap);
		}
	}

}

