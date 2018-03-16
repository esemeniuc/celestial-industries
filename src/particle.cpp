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

    template<typename T>
    T randomNumber(const T minimum, const T maximum) {
        std::uniform_real_distribution<T> distribution(minimum, maximum);
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
        if (mass <= 0){
            throw "Particles must have a mass > 0! Did you mean to use setInfiniteMass()?\n";
        }

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

        if (timeDelta <= 0.0) {
            throw "Particles can only be simulated forward in time! timeDelta must be > 0.\n";
        }

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

    void Particle::setEmitterId(unsigned long newEmitterId) {
        this->emitterId = newEmitterId;
    }

    void Particle::setAge(float newAge) {
        this->age = newAge;
    }

    void Particle::setPosition(glm::vec3 newPosition) {
        this->position = newPosition;
    }

    glm::vec3 Particle::getPosition() const {
        return position;
    }


    void Particle::setVelocity(glm::vec3 newVelocity) {
        this->velocity = newVelocity;
    }

    glm::vec3 Particle::getVelocity() const {
        return velocity;
    }

    void Particle::setDamping(float newDamping) {
        this->damping = newDamping;
    }

    void Particle::setAcceleration(glm::vec3 newAcceleration) {
        this->acceleration = newAcceleration;
    }

    glm::vec3 Particle::getAcceleration() const {
        return acceleration;
    }

    void Particle::initializePosition(glm::vec3 newPosition) {
        initialPosition = newPosition;
        Particle::setPosition(newPosition);

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
                this->getParticleSpeed()*(1.0f - variance),
                this->getParticleSpeed()*(1.0f + variance)
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

    void ParticleEmitter::setPosition(const glm::vec3 &newPosition) {
        ParticleEmitter::position = newPosition;
    }

    float ParticleEmitter::getSpread() const {
        return spread;
    }

    void ParticleEmitter::setSpread(float newSpread) {
        ParticleEmitter::spread = newSpread;
    }

    float ParticleEmitter::getParticleWidth() const {
        return particleWidth;
    }

    void ParticleEmitter::setParticleWidth(float newParticleWidth) {
        ParticleEmitter::particleWidth = newParticleWidth;
    }

    float ParticleEmitter::getParticleHeight() const {
        return particleHeight;
    }

    void ParticleEmitter::setParticleHeight(float newParticleHeight) {
        ParticleEmitter::particleHeight = newParticleHeight;
    }

    float ParticleEmitter::getParticleLifespan() const {
        return particleLifespan;
    }

    void ParticleEmitter::setParticleLifespan(float newParticleLifespan) {
        ParticleEmitter::particleLifespan = newParticleLifespan;
    }

    float ParticleEmitter::getParticleSpeed() const {
        return particleSpeed;
    }

    void ParticleEmitter::setParticleSpeed(float newParticleSpeed) {
        ParticleEmitter::particleSpeed = newParticleSpeed;
    }

    glm::vec3 ParticleEmitter::getDirection() const {
        return direction;
    }

    void ParticleEmitter::setDirection(glm::vec3 newDirection) {
        ParticleEmitter::direction = newDirection;
    }

    /**
     * Creates a new ParticleEmitter, an object in the world that emits a continuous-looking
     * stream of particles. Use this, not the ParticleEmitter class's constructor, to create
     * them.
     *
     * @param position xyz-coordinates to place the emitter on. y must be >= 0.
     * @param direction unit vector giving the direction that particles are emitted into
     * @param spread how far particles may vary from the direction vector
     * @param particleWidth width of a particle graphic
     * @param particleHeight height of a particle graphic
     * @param particleLifespan how long a particle may exist before being reset and respawned
     * @param particleSpeed how quickly this emitter's particles move
     * @return pointer to the created ParticleEmitter
     */
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
        if (position.y < 0) {
            throw "ParticleEmitters must be placed at a y-coordinate >= 0!\n";
        }

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
            particleEntity->scale(0, glm::vec3{0.1f});
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
