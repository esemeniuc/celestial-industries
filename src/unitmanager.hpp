#pragma once

#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		playerUnits.reserve(levelHeight * levelWidth);
		aiUnits.reserve(levelHeight * levelWidth);
	}

	void removeUnit(const std::shared_ptr<Entity>& unit) {
		if (unit->aiComp.owner == GamePieceOwner::PLAYER) {
			playerUnits.erase(std::remove(playerUnits.begin(), playerUnits.end(), unit), playerUnits.end());

		} else if (unit->aiComp.owner == GamePieceOwner::AI) {
			aiUnits.erase(std::remove(aiUnits.begin(), aiUnits.end(), unit), aiUnits.end());
		}
	}

	void update(double elapsed_ms) {
		for (auto& playerUnit : playerUnits) {
			playerUnit->unitComp.update();
			playerUnit->move(elapsed_ms);
			if (playerUnit.get()->aiComp.currentHealth <= 0) {
				removeUnit(playerUnit);
				logger << "Friendly Entity " << "Died!";
			}
		}

		for (auto& aiUnit : aiUnits) {
			aiUnit->unitComp.update();
			aiUnit->move(elapsed_ms);
			if (aiUnit.get()->aiComp.currentHealth <= 0) {
				removeUnit(aiUnit);
				logger << "AI Entity " << "Died!";
			}
		}
	}

}

