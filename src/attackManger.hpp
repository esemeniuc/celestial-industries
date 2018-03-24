//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

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

        for (std::shared_ptr<Entity> entity1 : aiUnits) {
            for (std::shared_ptr<Entity> entity2 : playerUnits) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                }
            }
        }

        for (std::shared_ptr<Entity> entity1 : aiUnits) {
            for (std::shared_ptr<Entity> entity2 : buildingMap) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                }
            }
        }

        for (std::shared_ptr<Entity> entity1 : playerUnits) {
            for (std::shared_ptr<Entity> entity2 : aiUnits) {
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

                pair.first->moveTo(pair.second->getPosition().x, pair.second->getPosition().z);
                if (pair.second->aiComp.currentHealth <= 0) {
                    unitTargetMap.erase(pair.first);
                }

            }
        }




    }
};