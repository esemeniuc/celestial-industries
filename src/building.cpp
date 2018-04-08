//
// Created by eric on 3/17/18.
//

#include "building.hpp"

std::shared_ptr<Entity>
Building::spawn(Building::BuildingType buildingType, glm::vec3 spawnLocation, GamePieceOwner owner) {
	std::shared_ptr<Entity> e = std::make_shared<Entity>();

	switch (buildingType) {
		case (BuildingType::SUPPLY_DEPOT): {
			e->geometryRenderer = Model::meshRenderers[Model::MINING_TOWER];//FIXME: use real thing

			e->aiComp.totalHealth = 400;
			e->aiComp.visionRange = 10;
			e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
			e->aiComp.value = 100;

			e->unitComp.state = UnitState::NONE;
			break;
		}
		case (BuildingType::REFINERY): {
			Coord location(spawnLocation);
			if(Global::levelArray[location.rowCoord][location.colCoord] != Model::GEYSER ){ //make sure it is placed on a geyser
				return nullptr; //FIXME: do something smarter than this
			}

			e->geometryRenderer = Model::meshRenderers[Model::MINING_TOWER];

			e->aiComp.totalHealth = 500;
			e->aiComp.visionRange = 10;
			e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
			e->aiComp.value = 75;

			e->unitComp.state = UnitState::NONE;
			break;
		}
		case (BuildingType::GUN_TURRET): {
			e->geometryRenderer = Model::meshRenderers[Model::GUN_TURRET];

			e->aiComp.totalHealth = 250;
			e->aiComp.visionRange = 10;
			e->aiComp.type = GamePieceClass::BUILDING_DEFENSIVE_ACTIVE;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
			e->aiComp.value = 100;

			e->unitComp.state = UnitState::NONE;
			break;
		}
		case (BuildingType::COMMAND_CENTER): {
			e->geometryRenderer = Model::meshRenderers[Model::PHOTON_TOWER];//FIXME: use real thing

			e->aiComp.totalHealth = 1500;
			e->aiComp.visionRange = 10;
			e->aiComp.type = GamePieceClass::BUILDING_NON_ATTACKING;
			e->aiComp.currentHealth = e->aiComp.totalHealth;
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
