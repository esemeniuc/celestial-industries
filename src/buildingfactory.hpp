#pragma once

#include "entity.hpp"

namespace BuildingFactory {

	enum class BuildingType {
		SUPPLY_DEPOT,
		REFINERY,
		GUN_TURRET,
		COMMAND_CENTER
	};

	Entity spawnUnit(BuildingType unitName, GamePieceOwner owner) {
		Entity e;
		switch (unitName) {
			case (BuildingType::SUPPLY_DEPOT): {
				e.aiComp.initialHealth = 400;
				e.aiComp.visionRange = 10;
				e.aiComp.owner = owner;
				e.aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e.aiComp.currentHealth = e.aiComp.initialHealth;
				e.aiComp.value = 100;

				e.unitComp.state = UnitState::NONE;
				return e;
			}
			case (BuildingType::REFINERY): {
				e.aiComp.initialHealth = 500;
				e.aiComp.visionRange = 10;
				e.aiComp.owner = owner;
				e.aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e.aiComp.currentHealth = e.aiComp.initialHealth;
				e.aiComp.value = 75;

				e.unitComp.state = UnitState::NONE;
				return e;
			}
			case (BuildingType::GUN_TURRET): {
				e.aiComp.initialHealth = 250;
				e.aiComp.visionRange = 10;
				e.aiComp.owner = owner;
				e.aiComp.type = GamePieceClass::BUILDING_DEFENSIVE_ACTIVE;
				e.aiComp.currentHealth = e.aiComp.initialHealth;
				e.aiComp.value = 100;

				e.unitComp.state = UnitState::NONE;
				return e;
			}
			case (BuildingType::COMMAND_CENTER): {
				e.aiComp.initialHealth = 1500;
				e.aiComp.visionRange = 10;
				e.aiComp.owner = owner;
				e.aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
				e.aiComp.currentHealth = e.aiComp.initialHealth;
				e.aiComp.value = 400;

				e.unitComp.state = UnitState::NONE;
				return e;
			}
		}
	}


};


