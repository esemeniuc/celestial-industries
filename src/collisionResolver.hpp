#pragma once
#include <math.h>
#include "rigidBody.hpp"
#include "config.hpp"

namespace CollisionDetection {
    
    // update a rigid body's position and velocity over an
    // integration period dt, by computing acceleration based
    // on Newton's law F = ma
    void simIntegrate(RigidBody& rigidBody,float integrationPeriod);
}