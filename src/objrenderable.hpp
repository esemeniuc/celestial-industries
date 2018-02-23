#pragma once
#include "common.hpp"

class OBJRenderable : public Renderable {
public:
    bool init(const OBJ::Data& obj);
    void draw(glm::mat4 viewProjection)override;
    void destroy();
    void translate(glm::vec3 translation);
};