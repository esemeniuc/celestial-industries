#pragma once
#include "common.hpp"

class OBJRenderable : public Renderable {
public:
    bool init(std::shared_ptr<std::vector<Mesh>> initMeshes, std::shared_ptr<Shader> shader);
    void draw(glm::mat4 viewProjection)override;
    void destroy();
    void translate(glm::vec3 translation);
};