//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include <unordered_map>
#include "global.hpp"

namespace AttackManager {
    void registerTargetUnit(std::shared_ptr<Entity> unit1, std::shared_ptr<Entity> unit2);

	void initiateAttacks(std::vector<std::shared_ptr<Entity>>& entities1, std::vector<std::shared_ptr<Entity>>& entities2, double elapsed_ms);

	bool isDead(const std::shared_ptr<Entity>& entity);

	void removeDeadEntities(std::vector<std::shared_ptr<Entity>>& entities);

	void executeTargetAttacks(double elapsed_ms);

    void update(double elapsed_ms);
}
