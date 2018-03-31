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

	std::shared_ptr<Entity> spawn(BuildingType buildingType, glm::vec3 spawnLocation, GamePieceOwner owner) {
		std::shared_ptr<Entity> e = std::make_shared<Entity>();

		switch (buildingType) {
			case (BuildingType::SUPPLY_DEPOT): {
				e->aiComp.initialHealth = 400;
				e->aiComp.visionRange = 10;
				e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 100;

				e->unitComp.state = UnitState::NONE;
				break;
			}
			case (BuildingType::REFINERY): {
				e->aiComp.initialHealth = 500;
				e->aiComp.visionRange = 10;
				e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 75;

				e->unitComp.state = UnitState::NONE;
				break;
			}
			case (BuildingType::GUN_TURRET): {
				e->aiComp.initialHealth = 250;
				e->aiComp.visionRange = 10;
				e->aiComp.type = GamePieceClass::BUILDING_DEFENSIVE_ACTIVE;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 100;

				e->unitComp.state = UnitState::NONE;
				break;
			}
			case (BuildingType::COMMAND_CENTER): {
				e->aiComp.initialHealth = 1500;
				e->aiComp.visionRange = 10;
				e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 400;

				e->unitComp.state = UnitState::NONE;
				break;
			}
		}

		Global::buildingMap.push_back(e);
		e->setPosition(spawnLocation);
		e->aiComp.owner = owner;
		return e;
	}

}