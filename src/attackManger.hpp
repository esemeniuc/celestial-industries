//
// Created by Gurjot Kheeva on 2018-03-17.
//
#pragma once

#include "global.hpp"

namespace AttackManager {

    void removeEntity(Entity &entity);
    void attackEntity(Entity &entity1, Entity &entity2, double elapsed_ms);
    void update(double elapsed_ms);

};