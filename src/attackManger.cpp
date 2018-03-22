//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include "attackManger.hpp"

std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> unitTargetMap;
double targetTime = 0;


void AttackManager::registerTargetUnit(std::shared_ptr<Entity> unit1, std::shared_ptr<Entity> unit2) {
    unitTargetMap.insert( {unit1, unit2} );
}

void AttackManager::update(double elapsed_ms) {
    std::vector<std::shared_ptr<Entity>> allEntities;

    for (std::shared_ptr<Entity> entity : aiUnits) {
        allEntities.push_back(entity);
    }

    for (std::shared_ptr<Entity> entity : playerUnits) {
        allEntities.push_back(entity);
    }

    for (std::shared_ptr<Entity> entity : buildingMap) {
        allEntities.push_back(entity);
    }

    logger(LogLevel::DEBUG) << "Number of entities found: " << allEntities.size() << " \n";
    logger << "MS passed: " << elapsed_ms << '\n';
    for (std::shared_ptr<Entity> entity1 : aiUnits) {
        for (std::shared_ptr<Entity> entity2 : playerUnits) {
            if (entity1->inAttackRange(entity2)) {
                // Entity1 attacks entity2 for elapsed_ms amount of time.
                logger << "New attack Started!\n";
                entity1->attack(entity2, elapsed_ms);
            } else {
                logger << "Entity Not in range or already attacking!\n";
            }
        }
    }

    for (std::shared_ptr<Entity> entity1 : aiUnits) {
        for (std::shared_ptr<Entity> entity2 : buildingMap) {
            if (entity1->inAttackRange(entity2)) {
                // Entity1 attacks entity2 for elapsed_ms amount of time.
                logger << "New attack Started!\n";
                entity1->attack(entity2, elapsed_ms);
            } else {
                logger << "Entity Not in range or already attacking!\n";
            }
        }
    }

    for (std::shared_ptr<Entity> entity1 : playerUnits) {
        for (std::shared_ptr<Entity> entity2 : aiUnits) {
            if (entity1->inAttackRange(entity2)) {
                // Entity1 attacks entity2 for elapsed_ms amount of time.
                logger << "New attack Started!\n";
                entity1->attack(entity2, elapsed_ms);
            } else {
                logger << "Entity Not in range or already attacking!\n";
            }
        }
    }

    for (auto& entity : allEntities) {
        entity->unitComp.state = UnitState::IDLE;
    }

    if (targetTime < 1000) {
        targetTime += elapsed_ms;
    } else targetTime = 0;

    if (targetTime == 0) {
        for (std::pair<std::shared_ptr<Entity>, std::shared_ptr<Entity>> pair : unitTargetMap) {

            pair.first->moveTo(pair.second->getPosition().x, pair.second->getPosition().z);
            if (pair.second.get()->aiComp.currentHealth <= 0) {
                //unitTargetMap.erase(pair.first);
            }

        }
    }




}
