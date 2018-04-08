//
// Created by eric on 3/16/18.
//

#include "unitmanager.hpp"

void UnitManager::init(size_t levelHeight, size_t levelWidth) {
	Global::aiVisibilityMap = std::vector<std::vector<int>>(levelHeight, std::vector<int>(levelWidth));
	Global::playerVisibilityMap = std::vector<std::vector<int>>(levelHeight, std::vector<int>(levelWidth));
}

bool UnitManager::isDead(std::shared_ptr<Entity>& unit) {
	if (unit->aiComp.currentHealth <= 0) {
		unit->softDelete();
	}
	return unit->aiComp.currentHealth <= 0;
}

void UnitManager::removeDead() {
	//std::cout << "before: " << playerUnits.size() << '\n';
	Global::playerUnits.erase(std::remove_if(Global::playerUnits.begin(), Global::playerUnits.end(), isDead),
							  Global::playerUnits.end());
	//std::cout << "after: " << playerUnits.size() << '\n';
	Global::aiUnits.erase(std::remove_if(Global::aiUnits.begin(), Global::aiUnits.end(), isDead),
						  Global::aiUnits.end());
}

void UnitManager::updateAreaSeenByUnit(const std::shared_ptr<Entity>& unit, int currentUnixTime,
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

void UnitManager::update(double elapsed_ms) {
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

bool isWithinBounds(const std::shared_ptr<Entity>& entity, const Coord& startCorner, const Coord& endCorner) {
	int colMin = std::min(startCorner.colCoord, endCorner.colCoord);
	int colMax = std::max(startCorner.colCoord, endCorner.colCoord);
	int rowMin = std::min(startCorner.rowCoord, endCorner.rowCoord);
	int rowMax = std::max(startCorner.rowCoord, endCorner.rowCoord);

	return entity->getPosition().x >= colMin &&
		   entity->getPosition().x <= colMax &&
		   entity->getPosition().z >= rowMin &&
		   entity->getPosition().z >= rowMax;
}

//highlight gets only friendly units, point click gets both friendly and enemy units
//since player might want to inpsect enemy unit
std::vector<std::shared_ptr<Entity>> UnitManager::selectUnitsInRange(Coord startCorner, Coord endCorner) {
	std::vector<std::shared_ptr<Entity>> unitsSelected;

	//add enemy units if just a point click
	if (Coord::l1Norm(startCorner, endCorner) < Config::POINT_CLICK_DISTANCE_THRESHOLD) {
		for (const auto& aiUnit : Global::aiUnits) {
			if (isWithinBounds(aiUnit, startCorner, endCorner)) { //start or end works since it is just a point
				unitsSelected.push_back(aiUnit);
			}
		}

	}

	for (const auto& playerUnit : Global::playerUnits) {
		if (isWithinBounds(playerUnit, startCorner, endCorner)) {//start or end works since it is just a point
			unitsSelected.push_back(playerUnit);
		}
	}

	return unitsSelected;
}
