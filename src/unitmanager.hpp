#pragma once

#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		playerUnits.reserve(levelHeight * levelWidth);
		aiUnits.reserve(levelHeight * levelWidth);
	}

	bool isDead( std::shared_ptr<Entity>& unit) //THIS IS A TERRIBLE HACK, FIXME
	{
		if(unit->aiComp.currentHealth <= 0)
		{
			//unit->translate({999,999,999});
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead() {
		std::cout << "before: " << playerUnits.size() << '\n';
		playerUnits.erase(std::remove_if(playerUnits.begin(), playerUnits.end(), isDead), playerUnits.end());
		std::cout << "after: " << playerUnits.size() << '\n';
		aiUnits.erase(std::remove_if(aiUnits.begin(), aiUnits.end(), isDead), aiUnits.end());
	}

	void update(double elapsed_ms) {
		removeDead();
		for (auto& playerUnit : playerUnits) {
			logger << "current HEALTH: " << playerUnit->aiComp.currentHealth << '\n';
				playerUnit->unitComp.update();
				playerUnit->move(elapsed_ms);
				//logger << "Friendly Entity " << "Died!";
		}

		for (auto& aiUnit : aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
		}
	}

}

