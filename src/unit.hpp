#pragma once

#include <memory>
#include "entity.hpp"
#include "global.hpp"

namespace Unit {

	enum class UnitType {
		SPHERICAL_DEATH,
		TANK,
		ZEALOT,
		MARINE
	};

	std::shared_ptr<Entity> spawn(UnitType unitName, glm::vec3 spawnLocation, GamePieceOwner owner);
}
