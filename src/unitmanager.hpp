#pragma once

#include <vector>
#include "global.hpp"
#include "aimanager.hpp"

namespace UnitManager {

	extern std::vector<std::shared_ptr<Entity>> selectedUnits;

	void init(size_t levelHeight, size_t levelWidth);

	bool isDead(std::shared_ptr<Entity>& unit);

	void removeDead();

	void updateAreaSeenByUnit(const std::shared_ptr<Entity>& unit, int currentUnixTime,
							  std::vector<std::vector<int>>& visibilityMap);

	void update(double elapsed_ms);

	void selectUnit(const glm::vec3& targetLocation);

	void selectUnitsInTrapezoid(const glm::vec3& topLeft, const glm::vec3& topRight,
								const glm::vec3& bottomLeft, const glm::vec3& bottomRight);

	void sortSelectedUnits(); //use this for displaying selected units (want to group them nicely)

	void attackTargetLocationWithSelectedUnits(const glm::vec3& targetLocation);
}

