//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include "global.hpp"

namespace AttackManager {

    void registerTargetUnit(std::shared_ptr<Entity> unit1, std::shared_ptr<Entity> unit2);
    void update(double elapsed_ms);
};