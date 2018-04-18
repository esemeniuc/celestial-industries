#pragma once

#include <algorithm>
#include "global.hpp"

namespace BuildingManager {
	void init(size_t levelHeight, size_t levelWidth);

	bool isDead(std::shared_ptr<Entity>& unit);

	void removeDead();

	void update(double elapsed_ms);


	void selectBuilding(const glm::vec3& targetLocation);
}
