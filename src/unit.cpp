//
// Created by eric on 3/17/18.
//

#include "unit.hpp"

// Sets AI comp and Unit comp
void initUnitFromMeshType(const std::shared_ptr<Entity>& e, Model::MeshType type, GamePieceOwner owner) {
	switch (type) {
		case Model::MeshType::FRIENDLY_FIRE_UNIT:
		case Model::MeshType::ENEMY_SPIKE_UNIT:
			e->aiComp.totalHealth = 160;
			e->aiComp.visionRange = 8;
			e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
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
			e->aiComp.totalHealth = 45;
			e->aiComp.visionRange = 8;
			e->aiComp.type = GamePieceClass::UNIT_OFFENSIVE;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
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

	} else if (owner == GamePieceOwner::AI) {
		Global::aiUnits.push_back(e);
	}
}

std::shared_ptr<Entity> Unit::spawn(Model::MeshType type, glm::vec3 spawnLocation, GamePieceOwner owner) {
	std::shared_ptr<Entity> entity = entityFromMeshType(type);
	entity->setPosition(spawnLocation);
	initUnitFromMeshType(entity, type, owner);
	return entity;
}

std::shared_ptr<Entity> Unit::entityFromMeshType(Model::MeshType type) {
	switch (type) {
		case Model::MeshType::FRIENDLY_RANGED_UNIT:
			return std::make_shared<ProjectileFiringGunEntity>(type, 2, Model::MeshType::BULLET, glm::vec3(0,0.5,0));
		case Model::MeshType::ENEMY_RANGED_RADIUS_UNIT:
			return std::make_shared<BeamFiringGunEntity>(type, 1, Model::MeshType::BEAM, glm::vec3(0, 0.8, 0));
		default:
			return std::make_shared<Entity>(type);
	}
}

