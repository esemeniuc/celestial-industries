//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include "global.hpp"

namespace AttackManager {
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> unitTargetMap;
	std::vector<std::shared_ptr<Entity>> attackingEntities;
    double targetTime = 0;
	int TARGET_UPDATE_INTERVAL_MS = 1000;

    void registerTargetUnit(std::shared_ptr<Entity> unit1, std::shared_ptr<Entity> unit2) {
        unitTargetMap.insert( {unit1, unit2} );
    }

	void initiateAttacks(std::vector<std::shared_ptr<Entity>> entities1, std::vector<std::shared_ptr<Entity>> entities2, double elapsed_ms) {
		for (std::shared_ptr<Entity> entity1 : entities1) {
			for (std::shared_ptr<Entity> entity2 : entities2) {
				if (entity1->inAttackRange(*entity2)) {
					// Entity1 attacks entity2 for elapsed_ms amount of time.
					entity1->attack(*entity2);
					entity2->takeAttack(*entity1, elapsed_ms);
					attackingEntities.push_back(entity1);
				}
			}
		}
	}

	void removeDeadEntities(std::vector<std::shared_ptr<Entity>> entities) {
		for (auto it = entities.begin(); it != entities.end();) {
			if ((*it)->aiComp.currentHealth <= 0) {
				(*it)->softDelete();
				// Do not advance iterator if removing current element from vector.
				it = entities.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void executeTargetAttacks(double elapsed_ms) {
		// Target Path is updated every second (1000ms)
		if (targetTime < TARGET_UPDATE_INTERVAL_MS) {
			targetTime += elapsed_ms;
		}
		else targetTime = 0;

		if (targetTime == 0) {
			for (auto it = unitTargetMap.cbegin(); it != unitTargetMap.cend();) {
				if (it->second->aiComp.currentHealth <= 0) {
					unitTargetMap.erase(it++);    // or "it = m.erase(it)" since C++11
				}
				else {
					it->first->moveTo(it->second->getPosition().x, it->second->getPosition().z);
					++it;
				}
			}
		}
	}

    void update(double elapsed_ms) {
		attackingEntities.clear();

		initiateAttacks(playerUnits, aiUnits, elapsed_ms);
		initiateAttacks(aiUnits, playerUnits, elapsed_ms);
		initiateAttacks(aiUnits, buildingMap, elapsed_ms);

		removeDeadEntities(playerUnits);
		removeDeadEntities(aiUnits);
		removeDeadEntities(buildingMap);

		executeTargetAttacks(elapsed_ms);

		// Set the attacking entities' state back to IDLE so that next frame they are avaliable to attack again.
		for (std::shared_ptr<Entity> entity : attackingEntities) {
			entity->unitComp.state = UnitState::IDLE;
		}
    }
};
