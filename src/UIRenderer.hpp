//
// Created by Peter Deltchev on 2018-03-21.
//

#ifndef PROJ_UIRENDERER_HPP
#define PROJ_UIRENDERER_HPP

#include <glm/vec3.hpp>
#include "renderer.hpp"


class UIRenderer {
public:
    UIRenderer() = delete;
    UIRenderer(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);

    void render(glm::mat4 viewProjection);
private:
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    std::shared_ptr<Shader> shader;
    std::shared_ptr<Texture> texture;
    GLuint positionAttribute;
    GLuint textureCoordinateAttribute;
    GLuint modelViewProjectionUniform;
};


#endif //PROJ_UIRENDERER_HPP
