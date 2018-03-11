#include <cmath>
#include <random>
#include "particle.h"


template <typename NumberType>
NumberType randomNumber(const NumberType minimum, const NumberType maximum) {
    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<NumberType> distribution(minimum, maximum);
    return distribution(randomGenerator);
}


glm::vec3 randomVector(const glm::vec3 minimum, const glm::vec3 maximum) {
    return {
            randomNumber<float>(minimum.x, maximum.x),
            randomNumber<float>(minimum.y, maximum.y),
            randomNumber<float>(minimum.z, maximum.z)
    };
}


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

void Particle::setType(int type) {
    this->type = type;
}

void Particle::setAge(float age) {
    this->age = age;
}

void Particle::setPosition(glm::vec3 position) {
    this->position = position;
}

glm::vec3 Particle::getPosition()const {
    return position;
}


void Particle::setVelocity(glm::vec3 velocity) {
    this->velocity = velocity;
}

glm::vec3 Particle::getVelocity() const {
    return velocity;
}

void Particle::setDamping(float damping) {
    this->damping = damping;
}

void Particle::setAcceleration(glm::vec3 acceleration) {
    this->acceleration = acceleration;
}

glm::vec3 Particle::getAcceleration() const {
    return acceleration;
}




void FireworkRule::setParameters(
        unsigned type,
        float minimumAge,
        float maximumAge,
        const glm::vec3 &minimumVelocity,
        const glm::vec3 &maximumVelocity,
        float damping
) {
    this->type = type;
    this->minimumAge = minimumAge;
    this->maximumAge = maximumAge;
    this->minimumVelocity = minimumVelocity;
    this->maximumVelocity = maximumVelocity;
    this->damping = damping;
}

void FireworkRule::create(Firework *firework, const Firework *parent = nullptr) const {
    firework->setType(type);

    firework->setAge(randomNumber<float>(minimumAge, maximumAge));

    glm::vec3 velocity = {};
    if (parent) {
        firework->setPosition(parent->getPosition());
    } else {
        // static starting position
        // TODO: revisit this
        firework->setPosition({20, 0, 20});
    }

    velocity += randomVector(minimumVelocity, maximumVelocity);
    firework->setVelocity(velocity);

    firework->setMass(1);
    firework->setDamping(damping);

    // TODO: revisit this
    firework->setAcceleration({0, 9.8, 0});

    // TODO: implement force accumulator
}
