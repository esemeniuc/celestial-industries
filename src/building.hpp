#pragma once

#include "entity.hpp"
#include "global.hpp"

namespace Building {

	enum class DefensiveBuildingState {
		IDLE, PIVOT, ATTACK
	};

	enum class BuildingType {
		SUPPLY_DEPOT,
		REFINERY,
		GUN_TURRET,
		COMMAND_CENTER
	};

	std::shared_ptr<Entity> spawn(Model::MeshType meshType, glm::vec3 spawnLocation, GamePieceOwner owner);

}
