//
// Created by Gurjot Kheeva on 2018-03-17.
//

#include <unordered_map>
#include <algorithm>
#include "global.hpp"
#include "attackManager.hpp"

namespace AttackManager {
    std::unordered_map<std::shared_ptr<Entity>, std::shared_ptr<Entity>> unitTargetMap;
    std::vector<std::shared_ptr<Entity>> attackingEntities;
    double targetTime = 0;
    int TARGET_UPDATE_INTERVAL_MS = 1000;

    void registerTargetUnit(std::shared_ptr<Entity>& unit1, std::shared_ptr<Entity>& unit2) {
        unitTargetMap.insert( {unit1, unit2} );
    }

    void executeAutoAttacks(std::vector<std::shared_ptr<Entity>> &entities1,
                            std::vector<std::shared_ptr<Entity>> &entities2, double elapsed_ms) {
        for (std::shared_ptr<Entity>& entity1 : entities1) {
            for (std::shared_ptr<Entity>& entity2 : entities2) {
                if (entity1->inAttackRange(entity2)) {
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                    attackingEntities.push_back(entity1);
                }
            }
        }
    }

    void executeAutoAttacksForBuildings(std::vector<std::shared_ptr<Entity>> &entities1,
                            std::vector<std::shared_ptr<Tile>> &entities2, double elapsed_ms) {

        for (std::shared_ptr<Entity>& entity1 : entities1) {
            for (std::shared_ptr<Tile>& entity2 : entities2) {
                if (entity1->inAttackRange(entity2)) {
                    //std::cout << "its runign \n";
                    // Entity1 attacks entity2 for elapsed_ms amount of time.
                    entity1->attack(entity2, elapsed_ms);
                    attackingEntities.push_back(entity1);
                }
            }
        }
    }

    bool isDead(const std::shared_ptr<Entity>& entity) {
        return (entity->aiComp.currentHealth <= 0);
    }

    void removeDeadEntities(std::vector<std::shared_ptr<Entity>>& entities) {
        std::remove_if(entities.begin(), entities.end(), isDead);
    }

    void executeTargetAttacks(double elapsed_ms) {
        // Target Path is updated every second (1000ms).
        if (targetTime < TARGET_UPDATE_INTERVAL_MS) {
            targetTime += elapsed_ms;
        }
        else targetTime = 0;

        if (targetTime == 0) {
            for (auto it = unitTargetMap.cbegin(); it != unitTargetMap.cend();) {
                if (it->second->aiComp.currentHealth <= 0) {
                    it->first->target = nullptr;
                    it->first->stopMoving();
					if (it->second->aiComp.owner == GamePieceOwner::PLAYER) {
						--Global::playerCurrentSupply;
					}
                    unitTargetMap.erase(it++);
                }
                else {
                    if (it->first->inAttackRange(it->second)) {
                        // In attack range. Set UnitState to IDLE (done in stopMoving) so that attack manager is
                        // aware that this entity is free to initiate attacks to AI unit.
                        it->first->stopMoving();
                    } else {
                        // Not in attack range yet. Move to get in MIN attack range.
                        // Set UnitState to ATTACK_MOVE indicating to the attack manager not to redirect this unit to do anything else.
                        it->first->moveTo(UnitState::ATTACK_MOVE, {it->second->getPosition().x, 0, it->second->getPosition().z}, false);
                    }
                    ++it;
                }
            }
        }
    }

    void update(double elapsed_ms) {
        attackingEntities.clear();

        executeTargetAttacks(elapsed_ms);

        executeAutoAttacks(Global::playerUnits, Global::aiUnits, elapsed_ms);
        executeAutoAttacks(Global::aiUnits, Global::playerUnits, elapsed_ms);
        executeAutoAttacksForBuildings(Global::aiUnits, Global::buildingTileList, elapsed_ms);

        //removeDeadEntities(Global::buildingTileList);

        // Set the attacking entities' state back to IDLE so that next frame they are available to attack again.
        for (std::shared_ptr<Entity>& entity : attackingEntities) {
            entity->unitComp.state = UnitState::IDLE;
        }
    }
}
