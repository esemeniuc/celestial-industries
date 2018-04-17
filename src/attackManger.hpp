//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include <unordered_map>

#include "global.hpp"

namespace AttackManager {
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> unitTargetMap;
    double targetTime = 0;

    void registerTargetUnit(std::shared_ptr<Entity> unit1, std::shared_ptr<Entity> unit2) {
        unitTargetMap.insert( {unit1, unit2} );
    }

    void update(double elapsed_ms) {
        // logger(LogLevel::DEBUG) << "Number of entities found: " << allEntities.size() << " \n";
        // logger << "MS passed: " << elapsed_ms << '\n';

        for (std::shared_ptr<Entity> entity1 : Global::aiUnits) {
            for (std::shared_ptr<Entity> entity2 : Global::playerUnits) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                }
            }
        }

        for (std::shared_ptr<Entity> entity1 : Global::aiUnits) {
            for (std::shared_ptr<Entity> entity2 : Global::buildingList) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                }
            }
        }

        for (std::shared_ptr<Entity> entity1 : Global::playerUnits) {
            for (std::shared_ptr<Entity> entity2 : Global::aiUnits) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                }
            }
        }

        // Target Path is updated every second (1000ms)
        if (targetTime < 1000) {
            targetTime += elapsed_ms;
        } else targetTime = 0;

        if (targetTime == 0) {
            for (std::pair<std::shared_ptr<Entity>, std::shared_ptr<Entity>> pair : unitTargetMap) {

                pair.first->moveTo(UnitState::ATTACK_MOVE, pair.second->getPosition());
                if (pair.second->aiComp.currentHealth <= 0) {
                    unitTargetMap.erase(pair.first);
                }

            }
        }




    }

	int const PRIORITIZE_CLOSER_ATTACKS = 2;

	std::shared_ptr<Entity>
	bestBuildingToAttack(std::vector<std::shared_ptr<Entity>>& buildings, Entity& entity) {
		float bestAttackValue = -1;
		std::shared_ptr<Entity> building;

		if (buildings.size() <= 0) {
			// TODO: take care of case where length of building list passed in is 0
		}

		for (auto& currentBuilding : buildings) {
			int buildingValue = currentBuilding->aiComp.value;

			float distanceToBuilding = 0;//getDistanceBetweenEntities(currentBuilding, entity); //fixme to revert

			float attackValue = buildingValue - (distanceToBuilding * PRIORITIZE_CLOSER_ATTACKS);

			if (attackValue > bestAttackValue) {
				bestAttackValue = attackValue;
				building = currentBuilding;
			}
		}

		return building;
	}

	std::shared_ptr<Entity> getHighestValuedBuilding(std::vector<std::shared_ptr<Entity>>& buildings) {
		int highestValueSoFar = -1;
		std::shared_ptr<Entity> building;

		if (buildings.size() <= 0) {
			// TODO: take care of case where length of building list passed in is 0
		}

		for (auto& currentBuilding : buildings) {
			if (currentBuilding->aiComp.value > highestValueSoFar) {
				highestValueSoFar = currentBuilding->aiComp.value;
				building = currentBuilding;
			}
		}

		return building;
	}
};
