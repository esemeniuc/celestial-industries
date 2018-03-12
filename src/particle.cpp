#include <cmath>
#include <random>
#include "particle.h"

namespace Particles {
    std::vector<Particle> particles;
    std::vector<std::shared_ptr<Entity>> particleGraphics;
    std::vector<FireworkRule> particleRules;

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
//        if (age > particleRules[type].maximumAge || position.y <= 0) {
//            resetParticle();
//        }
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

    void FireworkRule::create(Particle *firework, const Particle *parent = nullptr) const {
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
        firework->setAcceleration({0, -0.05, 0});

        // TODO: implement force accumulator
    }

    const glm::vec3 &ParticleSpawner::getPosition() const {
        return position;
    }

    void ParticleSpawner::setPosition(const glm::vec3 &position) {
        ParticleSpawner::position = position;
    }

    float ParticleSpawner::getSpread() const {
        return spread;
    }

    void ParticleSpawner::setSpread(float spread) {
        ParticleSpawner::spread = spread;
    }

    float ParticleSpawner::getParticleWidth() const {
        return particleWidth;
    }

    void ParticleSpawner::setParticleWidth(float particleWidth) {
        ParticleSpawner::particleWidth = particleWidth;
    }

    float ParticleSpawner::getParticleHeight() const {
        return particleHeight;
    }

    void ParticleSpawner::setParticleHeight(float particleHeight) {
        ParticleSpawner::particleHeight = particleHeight;
    }

    float ParticleSpawner::getParticleLifespan() const {
        return particleLifespan;
    }

    void ParticleSpawner::setParticleLifespan(float particleLifespan) {
        ParticleSpawner::particleLifespan = particleLifespan;
    }

    float ParticleSpawner::getParticleSpeed() const {
        return particleSpeed;
    }

    void ParticleSpawner::setParticleSpeed(float particleSpeed) {
        ParticleSpawner::particleSpeed = particleSpeed;
    }

    ParticleSpawner::ParticleSpawner(const glm::vec3 &position, const glm::vec3 &direction, float spread,
                                     float particleWidth, float particleHeight, float particleLifespan,
                                     float particleSpeed)
            : position(position), direction(direction), spread(spread), particleWidth(particleWidth),
              particleHeight(particleHeight), particleLifespan(particleLifespan), particleSpeed(particleSpeed) {

        this->createParticles();
    }

    void ParticleSpawner::renderParticles(float elapsed_ms) {
//        logger(LogLevel::DEBUG) << particles.size();

        for (size_t i = 0; i < particles.size(); ++i) {
            auto particle = &particles[i];
            auto particleEntity = particleGraphics[i];

            particle->updatePosition(elapsed_ms);
            particleEntity->setPosition(particle->getPosition());

//        glm::vec3 particlePosition = particleGraphics[i]->getPosition();
//        glm::vec3 particleVelocity = particles[i].getVelocity();
//        logger(LogLevel::DEBUG) << "particle[" << i << "] "
//                << particlePosition.x << ", " << particlePosition.y << ", " << particlePosition.z << " | "
//                << particleVelocity.x << ", " << particleVelocity.y << ", " << particleVelocity.z << '\n';
        }
    }

    void ParticleSpawner::createParticles() {
        // create ~1000 particle objects
        for (int i = 0; i < 3; ++i) {
            particles.emplace_back();
            particleRules[0].create(&particles[i], nullptr);
            particles[i].initializePosition({20, 0, 20});

            particleGraphics.push_back(std::make_shared<Entity>(Model::MeshType::BALL));
            particleGraphics[i]->translate(particles[i].getPosition());
            particleGraphics[i]->scale(glm::vec3(0.1, 0.1, 0.1));
        }
    }


    /**
     * This function defines all the particle rules.
     */
    void InitializeParticleSystem() {
        particleRules.emplace_back();
        particleRules.back().setParameters(
                0, // type
                0, 5, // age range
                glm::vec3(-0.1, 8 * 0.1, -0.1), // min velocity
                glm::vec3(0.1, 9 * 0.1, 0.1), // max velocity
                0.995 // damping
        );
    }
}
