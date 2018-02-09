#include "basicentity.hpp"

void BasicEntity::update(float ms)
{
    glm::vec3 direction = targetPosition - position;
    direction = glm::normalize(direction);
    direction.y = 0;
    position += direction * speed * ms;
    Entity::set_translation(position);
    model = glm::translate(glm::mat4(1.0f), position);
    return;
}

void BasicEntity::moveTo(glm::vec3 target)
{
    targetPosition = target;
}