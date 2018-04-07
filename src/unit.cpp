//
// Created by eric on 3/17/18.
//

#include "unit.hpp"


// Sets AI comp and Unit comp
void initUnitFromMeshType(std::shared_ptr<Entity> e, Model::MeshType type, GamePieceOwner owner) {
	switch (type) {
		case Model::MeshType::FRIENDLY_FIRE_UNIT:
		case Model::MeshType::ENEMY_SPIKE_UNIT:
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
		case Model::MeshType::FRIENDLY_RANGED_UNIT:
		case Model::MeshType::ENEMY_RANGED_RADIUS_UNIT:
		case Model::MeshType::BALL:
		case Model::MeshType::ENEMY_RANGED_LINE_UNIT:
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
		default:
			throw "Un initializeable unit encountered in initUnitFromMeshType";
	}

	e->aiComp.owner = owner;

	if (owner == GamePieceOwner::PLAYER) {
		Global::playerUnits.push_back(e);

	}
	else if (owner == GamePieceOwner::AI) {
		Global::aiUnits.push_back(e);
	}
}

std::shared_ptr<Entity> Unit::spawn(Model::MeshType type, glm::vec3 location, GamePieceOwner owner)
{
	std::shared_ptr<Entity> entity = entityFromMeshType(type);
	entity->setPosition(location);
	initUnitFromMeshType(entity, type, owner);
	return entity;
}

std::shared_ptr<Entity> Unit::entityFromMeshType(Model::MeshType type)
{
	switch (type) {
		case Model::MeshType::FRIENDLY_RANGED_UNIT:
			return std::make_shared<PivotingGunEntity>(type, 2);
		default:
			return std::make_shared<Entity>(type);
	}
}


std::shared_ptr<Entity> Unit::spawn(Unit::UnitType unitName, glm::vec3 spawnLocation, GamePieceOwner owner) {
	std::shared_ptr<Entity> e = std::make_shared<Entity>();
	e->setPosition(spawnLocation);

	switch (unitName) {
		case (UnitType::TANK): {
			e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

			e->aiComp.initialHealth = 150;
			e->aiComp.visionRange = 11;
			e->aiComp.type = GamePieceClass::UNIT_DEFENSIVE_ACTIVE;
			e->aiComp.currentHealth = e->aiComp.initialHealth;
			e->aiComp.value = 150 + 125;

			e->unitComp.initialEnergyLevel = 50;
			e->unitComp.attackDamage = 30;
			e->unitComp.attackRange = 5;
			e->unitComp.attackSpeed = 1;
			e->unitComp.movementSpeed = 20;
			e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
			e->unitComp.state = UnitState::IDLE;
			break;
		}
		case (UnitType::ZEALOT): {
			e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing


		}
		case (UnitType::MARINE): {
			e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

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
			e->unitComp.attackSpeed = 1;
			e->unitComp.movementSpeed = 1;
			e->unitComp.currentEnergyLevel = e->unitComp.initialEnergyLevel;
			e->unitComp.state = UnitState::IDLE;
			break;
		}
	}

	e->aiComp.owner = owner;

	if (owner == GamePieceOwner::PLAYER) {
		Global::playerUnits.push_back(e);

	} else if (owner == GamePieceOwner::AI) {
		Global::aiUnits.push_back(e);
	}
	return e;
}
