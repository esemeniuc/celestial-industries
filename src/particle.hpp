#ifndef PROJ_PARTICLE_H
#define PROJ_PARTICLE_H


#include <glm/vec3.hpp>
#include "renderer.hpp"
#include "entity.hpp"

namespace Particles {
    class ParticleEmitter {
    public:
        ParticleEmitter() = delete;
        ParticleEmitter(const ParticleEmitter &) = delete;
        ParticleEmitter(const ParticleEmitter &&) = delete;

        // TODO: see if the final argument being implicitly optional can be fixed
        ParticleEmitter(const glm::vec3 &position, const glm::vec3 &direction, float spread, float particleWidth,
                        float particleHeight, float particleLifespan, float particleSpeed, std::shared_ptr<Shader> shader,
                        std::shared_ptr<Texture> texture);

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

        void update(float elapsed_ms);
        void render(glm::mat4 viewProjection, glm::vec3 cameraPosition);

    private:
        // For rendering state
        GLuint vao;
        GLuint vbo;
        GLuint ibo;
        GLuint particleInstancesData;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<Texture> texture;
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
    };

}
#endif //PROJ_PARTICLE_H
