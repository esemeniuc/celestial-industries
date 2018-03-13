#include <cmath>
#include <random>
#include "particle.hpp"

#define PARTICLES_PER_EMITTER 200

namespace Particles {
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<Entity>> particleGraphics;
    std::vector<std::shared_ptr<ParticleEmitter>> particleEmitters;

    std::random_device r;
    std::default_random_engine randomGenerator(r());

    template<typename NumberType>
    NumberType randomNumber(const NumberType minimum, const NumberType maximum) {
        std::uniform_real_distribution<NumberType> distribution(minimum, maximum);
        return distribution(randomGenerator);
    }

    glm::vec3 randomVector(const glm::vec3 minimum, const glm::vec3 maximum) {
        return {
                randomNumber<float>(minimum.x, maximum.x),
                randomNumber<float>(minimum.y, maximum.y),
                randomNumber<float>(minimum.z, maximum.z)
        };
    }

    void updateParticleStates(float elapsed_ms) {
        for (const std::shared_ptr<ParticleEmitter> &emitter : particleEmitters) {
            emitter->updateParticlePositions(elapsed_ms);
        }
    }

    float Particle::getMass() {
        return 1.0f / inverseMass;
    }

    void Particle::setMass(float mass) {
        assert(mass > 0);
        inverseMass = 1.0f / mass;
    }

    void Particle::setInfiniteMass() {
        inverseMass = 0.0f;
    }

/**
 *
 * @param timeDelta number in milliseconds
 */
    void Particle::updatePosition(float timeDelta) {
        // internally, everything in the particle system is computed in terms of seconds.
        timeDelta *= 0.001;
        age += timeDelta;

        // TODO: consider removing the acceleration piece of this
//    position += velocity*timeDelta + acceleration*timeDelta*timeDelta*0.5;
        assert(timeDelta > 0.0);

        position += velocity * timeDelta;
        glm::vec3 newAcceleration = acceleration;
//    newAcceleration += forceAccum*inverseMass;
        velocity += newAcceleration * timeDelta;

        velocity *= pow(damping, timeDelta);


        // is the particle out of bounds? reset it if yes
        if (age > particleEmitters[emitterId]->getParticleLifespan() || position.y <= 0) {
            resetParticle();
        }
    }

    void Particle::setEmitterId(unsigned long emitterId) {
        this->emitterId = emitterId;
    }

    void Particle::setAge(float age) {
        this->age = age;
    }

    void Particle::setPosition(glm::vec3 position) {
        this->position = position;
    }

    glm::vec3 Particle::getPosition() const {
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

    void Particle::initializePosition(glm::vec3 position) {
        initialPosition = position;
        Particle::setPosition(position);

        initialVelocity = velocity;
        initialAcceleration = acceleration;
    }

    void Particle::resetParticle() {
        Particle::setPosition(initialPosition);
        Particle::setVelocity(initialVelocity);
        Particle::setAcceleration(initialAcceleration);
        Particle::setAge(0);
    }

    void ParticleEmitter::createParticle(Particle *particle) const {
        particle->setEmitterId(this->emitterId);
        particle->setAge(randomNumber<float>(0.0f, this->getParticleLifespan()));

        // given speed +/- variance governs the speeds that generated particles move at
        float variance = 0.2f;
        float speed = randomNumber<float>(
                this->getParticleSpeed()*(1.0f + variance),
                this->getParticleSpeed()*(1.0f - variance)
        );

        // TODO: pick vectors on the surface of a sphere here
        glm::vec3 velocity = randomVector(
                speed*this->getDirection() - glm::vec3{this->getSpread(), 0, this->getSpread()},
                speed*this->getDirection() + glm::vec3{this->getSpread(), 0, this->getSpread()});

        particle->setVelocity(velocity);

        particle->setMass(1);
        particle->setDamping(0.995);

        // TODO: revisit this
        particle->setAcceleration({0, -5, 0});

        // TODO: implement force accumulator
    }

    const glm::vec3 &ParticleEmitter::getPosition() const {
        return position;
    }

    void ParticleEmitter::setPosition(const glm::vec3 &position) {
        ParticleEmitter::position = position;
    }

    float ParticleEmitter::getSpread() const {
        return spread;
    }

    void ParticleEmitter::setSpread(float spread) {
        ParticleEmitter::spread = spread;
    }

    float ParticleEmitter::getParticleWidth() const {
        return particleWidth;
    }

    void ParticleEmitter::setParticleWidth(float particleWidth) {
        ParticleEmitter::particleWidth = particleWidth;
    }

    float ParticleEmitter::getParticleHeight() const {
        return particleHeight;
    }

    void ParticleEmitter::setParticleHeight(float particleHeight) {
        ParticleEmitter::particleHeight = particleHeight;
    }

    float ParticleEmitter::getParticleLifespan() const {
        return particleLifespan;
    }

    void ParticleEmitter::setParticleLifespan(float particleLifespan) {
        ParticleEmitter::particleLifespan = particleLifespan;
    }

    float ParticleEmitter::getParticleSpeed() const {
        return particleSpeed;
    }

    void ParticleEmitter::setParticleSpeed(float particleSpeed) {
        ParticleEmitter::particleSpeed = particleSpeed;
    }

    glm::vec3 ParticleEmitter::getDirection() const {
        return direction;
    }

    void ParticleEmitter::setDirection(glm::vec3 direction) {
        ParticleEmitter::direction = direction;
    }

    std::shared_ptr<ParticleEmitter> makeParticleEmitter(
            const glm::vec3 &position,
            const glm::vec3 &direction,
            float spread,
            float particleWidth,
            float particleHeight,
            float particleLifespan,
            float particleSpeed
    ) {
        auto emitter = std::make_shared<ParticleEmitter>(
                position,
                direction,
                spread,
                particleWidth,
                particleHeight,
                particleLifespan,
                particleSpeed
        );
        particleEmitters.push_back(emitter);
        return emitter;
    }

    ParticleEmitter::ParticleEmitter(
            const glm::vec3 &position,
            const glm::vec3 &direction,
            float spread,
            float particleWidth,
            float particleHeight,
            float particleLifespan,
            float particleSpeed
    ) : position(position),
        direction(direction),
        spread(spread),
        particleWidth(particleWidth),
        particleHeight(particleHeight),
        particleLifespan(particleLifespan),
        particleSpeed(particleSpeed)
    {
        // vertical positions less than 0 are invalid
        assert(position.y >= 0);

        if (position.y == 0) {
            this->position.y += 0.01; // to avoid resetting a particle first-thing
        }

        this->emitterId = particleEmitters.size();
        this->createParticles();
    }

    void ParticleEmitter::updateParticlePositions(float elapsed_ms) {
        size_t startingIndex = emitterId * PARTICLES_PER_EMITTER;
        size_t endIndex = startingIndex + PARTICLES_PER_EMITTER;

        for (size_t i = startingIndex; i < endIndex; ++i) {
            auto particle = &particles[i];
            auto particleEntity = particleGraphics[i];

            particle->updatePosition(elapsed_ms);
            particleEntity->setPosition(particle->getPosition());
            particleEntity->scale(0, glm::vec3{0.1});
        }
    }

    void ParticleEmitter::createParticles() {
        size_t numberOfParticles = particles.size();

        for (size_t i = numberOfParticles; i < (numberOfParticles + PARTICLES_PER_EMITTER); ++i) {
            particles.emplace_back();
            this->createParticle(&particles[i]);
            particles[i].initializePosition(position);

            particleGraphics.push_back(std::make_shared<Entity>(Model::MeshType::PARTICLE));
            particleGraphics[i]->translate(particles[i].getPosition());
        }
    }
}
