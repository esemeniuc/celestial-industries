//
// Created by Peter Deltchev on 2018-03-10.
//

#ifndef PROJ_PARTICLE_H
#define PROJ_PARTICLE_H


#include <glm/vec3.hpp>
#include "renderer.hpp"
#include "entity.hpp"

class Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    float damping;

public:

private:
    float getMass();
    void setMass(float mass);
    void setInfiniteMass();

    void updatePosition(float timeDelta);

private:
    float inverseMass;
};


class ParticleRules {
    unsigned int type;

    float minimumAge;

    float maximumAge;

    glm::vec3 minimumVelocity;

    glm::vec3 maximumVelocity;

    float damping;

    struct Payload {
        unsigned type;
        unsigned count;
    };

    unsigned payloadCount;

    Payload *payloads;
};

#endif //PROJ_PARTICLE_H
