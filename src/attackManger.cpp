//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include "attackManger.hpp"

// entity1 attacks entity2 for elapsed_ms amount of time.
void AttackManager::attackEntity(Entity &entity1, Entity &entity2, double elapsed_ms) {
    entity1.attack(entity2, elapsed_ms);



    if (entity1.aiComp.currentHealth <= 0) {
        removeEntity(entity2);
        logger << "Entity a " << "Died!";
    }

    if (entity2.aiComp.currentHealth <= 0) {
        entity1.unitComp.state = UnitState::IDLE;
        entity2.aiComp.currentHealth = 0;
        removeEntity(entity2);
        logger << "Entity b " << "Killed!";
    }
}

void AttackManager::update(double elapsed_ms) {

    std::vector<Entity> allEntities;

    for (std::shared_ptr<Entity> entity : entityMap) {
        if (entity->aiComp.type != GamePieceType::NONE) {
            allEntities.push_back(*entity);
        }
    }

    logger(LogLevel::INFO) << "Number of entities found: " << allEntities.size() << " \n";

    for (auto& entity1 : allEntities) {
        for (auto& entity2 : allEntities) {
            if (entity1.inAttackRange(entity2) && (entity1.unitComp.state != UnitState::ATTACK)) {
                attackEntity(entity1, entity2, elapsed_ms);
            }
        }
    }
}

void AttackManager::removeEntity(Entity &entity) {
    // TODO;
}
