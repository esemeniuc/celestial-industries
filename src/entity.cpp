#include "entity.hpp"

Entity::Entity() {}

Entity::Entity(Model::MeshType geometry) : geometryRenderer(Model::meshRenderers[geometry]){}

//example of using the animate function when overriding Entity
void Entity::animate(float ms)
{
    this->geometryRenderer.scale(glm::vec3(1.01, 1.01, 1.01)); //default implementation for demo purposes
}

void Entity::translate(int modelIndex, glm::vec3 translation)
{
    this->geometryRenderer.translate(modelIndex, translation);
}

void Entity::rotate(int modelIndex, float amount, glm::vec3 axis)
{
    this->geometryRenderer.rotate(modelIndex, amount, axis);
}

void Entity::scale(int modelIndex, glm::vec3 scale)
{
    this->geometryRenderer.scale(modelIndex, scale);
}

void Entity::setModelMatrix(int modelIndex, glm::mat4 mat)
{
    this->geometryRenderer.setModelMatrix(modelIndex, mat);
}

void Entity::setModelMatrix(int modelIndex, glm::vec3 translation, float angle, glm::vec3 rotationAxis, glm::vec3 scale)
{
    this->geometryRenderer.setModelMatrix(modelIndex, translation, angle, rotationAxis, scale);
}

void Entity::translate(glm::vec3 translation)
{
    this->geometryRenderer.translate(translation);
}

void Entity::rotate(float amount, glm::vec3 axis)
{
    this->geometryRenderer.rotate(amount, axis);
}

void Entity::scale(glm::vec3 scale)
{
    this->geometryRenderer.scale(scale);
}
