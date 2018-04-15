//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include <unordered_map>
#include "global.hpp"
#include "attackManger.hpp"

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
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2);
                    entity2->takeAttack(entity1, elapsed_ms);
                    attackingEntities.push_back(entity1);
                }
            }
        }
    }

    bool isDead(const std::shared_ptr<Entity>& entity) {
        return (entity->aiComp.currentHealth <= 0);
    }

    void removeDeadEntities(std::vector<std::shared_ptr<Entity>> entities) {
        std::remove_if(entities.begin(), entities.end(), isDead);
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
                    it->first->moveTo(UnitState::ATTACK, it->second->getPosition().x, it->second->getPosition().z);
                    ++it;
                }
            }
        }
    }

    void update(double elapsed_ms) {
        attackingEntities.clear();

        initiateAttacks(Global::playerUnits, Global::aiUnits, elapsed_ms);
        initiateAttacks(Global::aiUnits, Global::playerUnits, elapsed_ms);
        initiateAttacks(Global::aiUnits, Global::buildingMap, elapsed_ms);

        removeDeadEntities(Global::playerUnits);
        removeDeadEntities(Global::aiUnits);
        removeDeadEntities(Global::buildingMap);

        executeTargetAttacks(elapsed_ms);

        // Set the attacking entities' state back to IDLE so that next frame they are avaliable to attack again.
        for (std::shared_ptr<Entity> entity : attackingEntities) {
            entity->unitComp.state = UnitState::IDLE;
        }
    }
};


