#pragma once

#include <vector>
#include "global.hpp"
#include "aimanager.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		playerUnits.reserve(levelHeight * levelWidth);
		aiUnits.reserve(levelHeight * levelWidth);
	}

	bool isDead( std::shared_ptr<Entity>& unit)
	{
		if(unit->aiComp.currentHealth <= 0)
		{
			unit->softDelete();
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead() {
		//std::cout << "before: " << playerUnits.size() << '\n';
		playerUnits.erase(std::remove_if(playerUnits.begin(), playerUnits.end(), isDead), playerUnits.end());
		//std::cout << "after: " << playerUnits.size() << '\n';
		aiUnits.erase(std::remove_if(aiUnits.begin(), aiUnits.end(), isDead), aiUnits.end());
	}

	void update(double elapsed_ms) {
		removeDead();
		int currentUnixTime = (int) getUnixTime();
		for (auto& playerUnit : playerUnits) {
				playerUnit->unitComp.update();
				playerUnit->move(elapsed_ms);
				AiManager::updateAreaSeenByUnit(playerUnit, currentUnixTime, playerVisibilityMap);
		}

		for (auto& aiUnit : aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
			AiManager::updateAreaSeenByUnit(aiUnit, currentUnixTime, aiVisibilityMap);
		}
	}

}

