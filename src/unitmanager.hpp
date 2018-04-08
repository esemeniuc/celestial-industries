#pragma once

#include <vector>
#include "global.hpp"
#include "aimanager.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth);

	bool isDead(std::shared_ptr<Entity>& unit);

	void removeDead();

	void updateAreaSeenByUnit(const std::shared_ptr<Entity>& unit, int currentUnixTime,
							  std::vector<std::vector<int>>& visibilityMap);

	void update(double elapsed_ms);

	std::vector<std::shared_ptr<Entity>> selectUnitsInRange(Coord startCorner, Coord endCorner);

}

