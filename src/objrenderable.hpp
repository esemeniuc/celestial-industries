#pragma once
#include "common.hpp"

class OBJRenderable {
public:
    bool init(std::shared_ptr<std::vector<Mesh>> initMeshes, std::shared_ptr<Shader> shader);
    virtual void draw(glm::mat4 viewProjection);
    void destroy();
    void translate(glm::vec3 translation);

    std::shared_ptr<std::vector<Mesh>> meshes;
    std::shared_ptr<Shader> shader;
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix
};
