#include <cmath>
#include "particle.h"

float Particle::getMass() {
    return 1.0f / inverseMass;
}

void Particle::setMass(float mass) {
    inverseMass = 1.0f / mass;
}

void Particle::setInfiniteMass() {
    inverseMass = 0.0f;
}

void Particle::updatePosition(float timeDelta) {
    // TODO: consider removing the acceleration piece of this
//    position += velocity*timeDelta + acceleration*timeDelta*timeDelta*0.5;
    assert(timeDelta > 0.0);

    position += velocity*timeDelta;
//    glm::vec3 newAcceleration = acceleration;
//    newAcceleration += forceAccum*inverseMass;
//    velocity += newAcceleration*timeDelta;

    velocity *= pow(damping, timeDelta);
}
