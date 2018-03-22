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

	std::shared_ptr<Entity> spawn(UnitType unitName, glm::vec3 spawnLocation, GamePieceOwner owner) {
		std::shared_ptr<Entity> e = std::make_shared<Entity>();

		switch (unitName) {
			case (UnitType::TANK): {
				e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

				e->aiComp.initialHealth = 150;
				e->aiComp.visionRange = 11;
				e->aiComp.type = GamePieceClass::UNIT_DEFENSIVE_ACTIVE;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 150 + 125;

				e->unitComp.initialEnergyLevel = 50;
				e->unitComp.attackDamage = 50;
				e->unitComp.attackRange = 5;
				e->unitComp.attackSpeed = 1;
				e->unitComp.movementSpeed = 2;
				e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
				e->unitComp.state = UnitState::IDLE;
				break;
			}
			case (UnitType::ZEALOT): {
				e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

				e->aiComp.initialHealth = 160;
				e->aiComp.visionRange = 8;
				e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 100;

				e->unitComp.initialEnergyLevel = 50;
				e->unitComp.attackDamage = 10;
				e->unitComp.attackRange = 1;
				e->unitComp.attackSpeed = 5;
				e->unitComp.movementSpeed = 3;
				e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
				e->unitComp.state = UnitState::IDLE;
				break;
			}
			case (UnitType::MARINE): {
				e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

				e->aiComp.initialHealth = 45;
				e->aiComp.visionRange = 8;
				e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 50;

				e->unitComp.initialEnergyLevel = 50;
				e->unitComp.attackDamage = 6;
				e->unitComp.attackRange = 5;
				e->unitComp.attackSpeed = 5;
				e->unitComp.movementSpeed = 3;
				e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
				e->unitComp.state = UnitState::IDLE;
				break;
			}
			case (UnitType::SPHERICAL_DEATH): {
				e->geometryRenderer = Model::meshRenderers[Model::BALL];

				e->aiComp.initialHealth = 100;
				e->aiComp.visionRange = 5;
				e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
				e->aiComp.currentHealth = e->aiComp.initialHealth;
				e->aiComp.value = 1000;

				e->unitComp.initialEnergyLevel = 50;
				e->unitComp.attackDamage = 10;
				e->unitComp.attackRange = 1;
				e->unitComp.attackSpeed = 2;
				e->unitComp.movementSpeed = 5;
				e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
				e->unitComp.state = UnitState::IDLE;
				break;
			}
		}

		e->setPosition(spawnLocation);
		e->aiComp.owner = owner;

		if (owner == GamePieceOwner::PLAYER) {
			playerUnits.push_back(e);

		} else if (owner == GamePieceOwner::AI) {
			aiUnits.push_back(e);
		}
		return e;
	}
}