//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include "attackManger.hpp"

void AttackManager::update(double elapsed_ms) {

    for (std::shared_ptr<Entity> entity : aiUnits) {
        allEntities.push_back(entity);
    }

    for (std::shared_ptr<Entity> entity : playerUnits) {
        allEntities.push_back(entity);
    }

    for (std::shared_ptr<Entity> entity : buildingMap) {
        allEntities.push_back(entity);
    }

    logger(LogLevel::INFO) << "Number of entities found: " << allEntities.size() << " \n";

    for (std::shared_ptr<Entity> entity1 : allEntities) {
        for (std::shared_ptr<Entity> entity2 : allEntities) {
            if (entity1->inAttackRange(entity2)) {
                // Entity1 attacks entity2 for elapsed_ms amount of time.
                entity1->attack(entity2, elapsed_ms);
            }
        }
    }
}
