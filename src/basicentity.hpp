#pragma once
#include "entity.hpp"
#include "objloader.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// A basic entity does nothing on its own
class BasicEntity : public Entity {
public:
    void update(float ms);
    glm::vec3 position = { 0,0,0 };
    float speed = 0.001;
    void moveTo(glm::vec3 target);
    glm::vec3 targetPosition;
};