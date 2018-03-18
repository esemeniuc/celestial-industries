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
            float particleSpeed,
            std::shared_ptr<Shader> shader
    );

    class Particle {
    public:
        float getMass();
        void setMass(float mass);
        void setInfiniteMass();
        void setDamping(float newDamping);
        void setEmitterId(unsigned long newEmitterId);
        void setAge(float newAge);
        void setPosition(glm::vec3 newPosition);
        void initializePosition(glm::vec3 newPosition);
        glm::vec3 getPosition() const;
        void setVelocity(glm::vec3 newVelocity);
        glm::vec3 getVelocity() const;
        void setAcceleration(glm::vec3 newAcceleration);
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

        // TODO: see if the final argument being implicitly optional can be fixed
        ParticleEmitter(const glm::vec3 &position, const glm::vec3 &direction, float spread,
                                float particleWidth, float particleHeight, float particleLifespan,
                                float particleSpeed, std::shared_ptr<Shader> shader);

        float getSpread() const;
        void setSpread(float newSpread);
        glm::vec3 getDirection() const;
        void setDirection(glm::vec3 newDirection);
        float getParticleWidth() const;
        void setParticleWidth(float newParticleWidth);
        float getParticleHeight() const;
        void setParticleHeight(float newParticleHeight);
        float getParticleLifespan() const;
        void setParticleLifespan(float newParticleLifespan);
        float getParticleSpeed() const;
        void setParticleSpeed(float newParticleSpeed);
        const glm::vec3 &getPosition() const;
        void setPosition(const glm::vec3 &newPosition);

        void updateParticlePositions(float elapsed_ms);

        void update(float elapsed_ms);
        void render(glm::mat4 viewProjection, glm::vec3 cameraPosition);

    private:
        // For rendering state
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLuint particleInstancesData;
        std::shared_ptr<Shader> shader;
        GLuint positionAttribute;
        GLuint textureCoordinateAttribute;
        GLuint timeElapsedUniform;
        GLuint modelViewProjectionUniform;

        glm::vec3 position;
        glm::vec3 direction;
        float spread;
        float particleWidth;
        float particleHeight;
        float particleLifespan;
        float particleSpeed;

        float ageInMilliseconds;

        // index into the ParticleEmitters data structure
        unsigned long emitterId;

        void createParticles();
        void createParticle(Particle *particle) const;

    };

}
#endif //PROJ_PARTICLE_H
