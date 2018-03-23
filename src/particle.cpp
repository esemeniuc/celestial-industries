#include <cmath>
#include <utility>
#include "particle.hpp"

namespace Particles {
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
     * @param shader
     * @param texture
     */
    ParticleEmitter::ParticleEmitter(const glm::vec3 &position, const glm::vec3 &direction, float spread, float particleWidth,
                                     float particleHeight, float particleLifespan, float particleSpeed, std::shared_ptr<Shader> shader,
                                     std::shared_ptr<Texture> texture) :

            position(position),
            direction(direction),
            spread(spread),
            particleWidth(particleWidth),
            particleHeight(particleHeight),
            particleLifespan(particleLifespan),
            particleSpeed(particleSpeed),
            ageInMilliseconds(0),
            shader(shader),
            texture(std::move(texture))
    {
        // generate VAO to link VBO and VIO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // generate VBO to store the triangle vertices
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        TexturedVertex vertices[4] = {
                // top left corner
                {{-0.5f * particleWidth, 0.5f * particleHeight, 0}, {0, 1}},
                // top right corner
                {{0.5f * particleWidth,  0.5f * particleHeight, 0}, {1, 1}},
                // bottom left corner
                {{-0.5f * particleWidth, -0.5f * particleHeight, 0}, {0, 0}},
                // bottom right corner
                {{0.5f * particleWidth,  -0.5f * particleHeight, 0}, {1, 0}}
        };

        glBufferData(GL_ARRAY_BUFFER, 4*sizeof(TexturedVertex), &vertices, GL_STATIC_DRAW);

        // generate the IBO
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // triangles with their vertices in counterclockwise order
        int vertexIndices[6] = {
                // triangle 1
                0, 2, 3,
                // triangle 2
                0, 3, 1
        };

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(int), &vertexIndices, GL_STATIC_DRAW);

        auto temp1 = glGetAttribLocation(shader->program, "position");
        auto temp2 = glGetAttribLocation(shader->program, "textureCoordinate");

        positionAttribute = static_cast<GLuint>(temp1);
        textureCoordinateAttribute = static_cast<GLuint>(temp2);

        glEnableVertexAttribArray(positionAttribute);
        glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*) 0);

        glEnableVertexAttribArray(textureCoordinateAttribute);
        glVertexAttribPointer(textureCoordinateAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*) sizeof(glm::vec3));

        timeElapsedUniform = static_cast<GLuint>(glGetUniformLocation(shader->program, "timeElapsedSinceStart"));
        modelViewProjectionUniform = static_cast<GLuint>(glGetUniformLocation(shader->program, "modelViewProjection"));

        // prevent clobbering of our VAO
        glBindVertexArray(0);
    }

    void ParticleEmitter::update(float elapsed_ms) {
        ageInMilliseconds += elapsed_ms;
    }

    void ParticleEmitter::render(glm::mat4 viewProjection, glm::vec3 cameraPosition) {
        glUseProgram(shader->program);
        glBindVertexArray(vao);

        // pass parameters into the shader
//        glm::vec3 directionToCamera = cameraPosition - position;
//        glm::mat4 rotationMatrix = glm::lookAt(glm::vec3(0), cameraPosition, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

        glm::mat4 modelViewProjection = viewProjection * modelMatrix;

        glUniformMatrix4fv(modelViewProjectionUniform, 1, GL_FALSE, &modelViewProjection[0][0]);
        glUniform1f(timeElapsedUniform, ageInMilliseconds);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->id);

        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 2000);

        glDisable(GL_BLEND);
    }
}
