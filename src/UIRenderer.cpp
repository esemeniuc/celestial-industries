#include "UIRenderer.hpp"

UIRenderer::UIRenderer(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture) :
    shader(shader),
    texture(texture)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    TexturedVertex vertices[4] = {
            // top left corner
            {{-0.5f * 1.0f, 0.5f * 1.0f, 0}, {0, 1}},
            // top right corner
            {{0.5f * 1.0f,  0.5f * 1.0f, 0}, {1, 1}},
            // bottom left corner
            {{-0.5f * 1.0f, -0.5f * 1.0f, 0}, {0, 0}},
            // bottom right corner
            {{0.5f * 1.0f,  -0.5f * 1.0f, 0}, {1, 0}}
    };


    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(TexturedVertex), &vertices, GL_STATIC_DRAW);

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

    modelViewProjectionUniform = static_cast<GLuint>(glGetUniformLocation(shader->program, "modelViewProjection"));

    // prevent clobbering of our VAO
    glBindVertexArray(0);
}

void UIRenderer::render(glm::mat4 viewProjection) {
    glUseProgram(shader->program);
    glBindVertexArray(vao);

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1, 1, 1));
    glm::mat4 modelViewProjection = viewProjection * modelMatrix;

    glUniformMatrix4fv(modelViewProjectionUniform, 1, GL_FALSE, &modelViewProjection[0][0]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->id);


    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, 1);

    glDisable(GL_BLEND);
}
