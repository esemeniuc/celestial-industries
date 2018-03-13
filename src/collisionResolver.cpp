#include "collisionResolver.hpp"

namespace CollisionDetection {   
    void simIntegrate(RigidBody & rigidBody, float integrationPeriod)
    {
        glm::vec3 startPosition = rigidBody.getPosition();
        glm::vec3 startVelocity = rigidBody.getVelocity();

        // calculate new position
        glm::vec3 newPosition = startPosition + integrationPeriod * startVelocity;

        // compute acceleration from force applied and the rigid body's mass
        glm::vec3 acceleration;
        glm::vec3 force = rigidBody.getForce();
        float inverseMass = rigidBody.getInverseMass();
        acceleration = force * inverseMass;

        // calculat new velocity
        glm::vec3 newVelocity = startVelocity + acceleration * integrationPeriod;
        // scale it with a damping amount
        newVelocity *= powf(rigidBody.getDampingFactor(), integrationPeriod);

        // update rigid body's position and velocity
        rigidBody.setPosition(newPosition);
        rigidBody.setVelocity(newVelocity);
    }
}