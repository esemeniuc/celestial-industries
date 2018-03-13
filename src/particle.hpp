#ifndef PROJ_PARTICLE_H
#define PROJ_PARTICLE_H


#include <glm/vec3.hpp>
#include "renderer.hpp"
#include "entity.hpp"

namespace Particles {
    class Particle;
    class ParticleEmitter;

    void updateParticleStates(float elapsed_ms);
    std::shared_ptr<ParticleEmitter> makeParticleEmitter(
            const glm::vec3 &position,
            const glm::vec3 &direction,
            float spread,
            float particleWidth,
            float particleHeight,
            float particleLifespan,
            float particleSpeed
    );

    class Particle {
    public:
        float getMass();
        void setMass(float mass);
        void setInfiniteMass();
        void setDamping(float damping);
        void setEmitterId(unsigned long emitterId);
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
        unsigned long emitterId;
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


    class ParticleEmitter {
    public:
        ParticleEmitter() = delete;
        ParticleEmitter(const ParticleEmitter &) = delete;
        ParticleEmitter(const ParticleEmitter &&) = delete;

        ParticleEmitter(
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
        glm::vec3 getDirection() const;
        void setDirection(glm::vec3 direction);
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

        void updateParticlePositions(float elapsed_ms);

    private:
        glm::vec3 position;
        glm::vec3 direction;
        float spread;
        float particleWidth;
        float particleHeight;
        float particleLifespan;
        float particleSpeed;

        // index into the ParticleEmitters data structure
        unsigned long emitterId;

        void createParticles();
        void createParticle(Particle *particle) const;
    };

}
#endif //PROJ_PARTICLE_H
