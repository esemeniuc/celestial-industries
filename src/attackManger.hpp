//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include "global.hpp"

namespace AttackManager {
    std::vector<std::shared_ptr<Entity>> allEntities;

    void update(double elapsed_ms);
};