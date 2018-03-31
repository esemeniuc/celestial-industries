#pragma once

#include <vector>
#include "global.hpp"
#include "aimanager.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		Global::playerUnits.reserve(levelHeight * levelWidth);
		Global::aiUnits.reserve(levelHeight * levelWidth);
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
		Global::playerUnits.erase(std::remove_if(Global::playerUnits.begin(), Global::playerUnits.end(), isDead), Global::playerUnits.end());
		//std::cout << "after: " << playerUnits.size() << '\n';
		Global::aiUnits.erase(std::remove_if(Global::aiUnits.begin(), Global::aiUnits.end(), isDead), Global::aiUnits.end());
	}

	void update(double elapsed_ms) {
		removeDead();
		int currentUnixTime = (int) getUnixTime();
		for (auto& playerUnit : Global::playerUnits) {
				playerUnit->unitComp.update();
				playerUnit->move(elapsed_ms);
				AiManager::updateAreaSeenByUnit(playerUnit, currentUnixTime, Global::playerVisibilityMap);
		}

		for (auto& aiUnit : Global::aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
			AiManager::updateAreaSeenByUnit(aiUnit, currentUnixTime, Global::aiVisibilityMap);
		}
	}

}

