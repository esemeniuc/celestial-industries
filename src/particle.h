//
// Created by Peter Deltchev on 2018-03-10.
//

#ifndef PROJ_PARTICLE_H
#define PROJ_PARTICLE_H


#include <glm/vec3.hpp>
#include "renderer.hpp"
#include "entity.hpp"

namespace Particles {

    class Particle;
    struct FireworkRule;
    class ParticleSpawner;

    class Particle {
    public:
        float getMass();

        void setMass(float mass);

        void setInfiniteMass();

        void setDamping(float damping);

        void setType(int type);

        void setAge(float age);

        void setPosition(glm::vec3 position);

        void initializePosition(glm::vec3 position);

        glm::vec3 getPosition() const;

        void setVelocity(glm::vec3 velocity);

        glm::vec3 getVelocity() const;

        void setAcceleration(glm::vec3 acceleration);

        glm::vec3 getAcceleration() const;

        void updatePosition(float timeDelta);

        /**
         * Return true if the particle needs removal.
         *
         * @param timeDelta
         * @return
         */
        bool update(float timeDelta) {
            updatePosition(timeDelta);
            age -= timeDelta;
            return age < 0;
        }

    private:
        int type;
        float age;
        float inverseMass;

        glm::vec3 position;
        glm::vec3 initialPosition;
        glm::vec3 velocity;
        glm::vec3 initialVelocity;
        glm::vec3 acceleration;
        glm::vec3 initialAcceleration;

        float damping;

        void resetParticle();

    };

    struct FireworkRule {
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

        FireworkRule()
                :
                payloadCount(0),
                payloads(NULL) {
        }

        void init(unsigned payloadCount) {
            this->payloadCount = payloadCount;
            payloads = new Payload[payloadCount];
        }

        ~FireworkRule() {
            if (payloads != nullptr) delete[] payloads;
        }

        void setParameters(unsigned int type, float minimumAge, float maximumAge, const glm::vec3 &minimumVelocity,
                           const glm::vec3 &maximumVelocity, float damping);

        void create(Particle *firework, const Particle *parent) const;
    };


    class ParticleSpawner {
    public:
        ParticleSpawner() = delete;
        ParticleSpawner(const ParticleSpawner &) = delete;
        ParticleSpawner(const ParticleSpawner &&) = delete;

        ParticleSpawner(
                const glm::vec3 &position,
                const glm::vec3 &direction,
                float spread,
                float particleWidth,
                float particleHeight,
                float particleLifespan,
                float particleSpeed
        );

        float getSpread() const;
        void setSpread(float spread);
        float getParticleWidth() const;
        void setParticleWidth(float particleWidth);
        float getParticleHeight() const;
        void setParticleHeight(float particleHeight);
        float getParticleLifespan() const;
        void setParticleLifespan(float particleLifespan);
        float getParticleSpeed() const;
        void setParticleSpeed(float particleSpeed);
        const glm::vec3 &getPosition() const;
        void setPosition(const glm::vec3 &position);

        void renderParticles(float elapsed_ms);

    private:
        glm::vec3 position;
        glm::vec3 direction;
        float spread;
        float particleWidth;
        float particleHeight;
        float particleLifespan;
        float particleSpeed;

        unsigned int activeParticles;

        void createParticles();
    };

    void InitializeParticleSystem();
}
#endif //PROJ_PARTICLE_H
