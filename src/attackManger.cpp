//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include "attackManger.hpp"

// entity1 attacks entity2 for elapsed_ms amount of time.
void AttackManager::attackEntity(Entity &entity1, Entity &entity2, double elapsed_ms) {
    entity1.attack(entity2, elapsed_ms);



    if (entity1.aiComp.currentHealth <= 0) {
        removeEntity(entity2);

    }

    if (entity2.aiComp.currentHealth <= 0) {
        entity1.unitComp.state = UnitState::IDLE;
        entity2.aiComp.currentHealth = 0;
        removeEntity(entity2);
        logger << "Entity b " << "Killed!";
    }
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



    logger(LogLevel::INFO) << "Number of entities found: " << allEntities.size() << " \n";

    for (auto& entity1 : allEntities) {
        for (std::shared_ptr<Entity> auto entity2 : allEntities) {
            if (entity1->inAttackRange(*entity2)) {
                attackEntity(*entity1, *entity2, elapsed_ms);
            }
        }
    }
}

void AttackManager::removeEntity(Entity &entity) {
    // TODO;
}
