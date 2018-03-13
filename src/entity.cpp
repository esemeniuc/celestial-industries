#include "entity.hpp"

Entity::Entity(Model::MeshType geometry) : geometryRenderer(Model::meshRenderers[geometry]) {}

//example of using the animate function when overriding Entity
void Entity::animate(float ms)
{
    this->geometryRenderer.scale(glm::vec3(1.01, 1.01, 1.01)); //default implementation for demo purposes
}

void Entity::translate(int modelIndex, glm::vec3 translation)
{
    // when an entity is translated, translate both the geometry being rendered
    // and the (invisible) collision geometry with it
    this->geometryRenderer.translate(modelIndex, translation);
    this->rigidBody.setPosition(this->rigidBody.getPosition() + translation);
}

void Entity::rotate(int modelIndex, float amount, glm::vec3 axis)
{
    // when an entity is rotated, rotate both the geometry being rendered
    // and the (invisible) collision geometry with it
    this->geometryRenderer.rotate(modelIndex, axis);
    this->rigidBody.setRotation(this->rigidBody.getRotation(axis) + amount, axis);
}

void Entity::scale(int modelIndex, glm::vec3 scale)
{
    // this may require manually updating the collision geometry size
    // but I don't expect to have an entity being scaled multiple times
    // beyond when they are first loaded in our game world
    this->geometryRenderer.scale(modelIndex, scale);
}

void Entity::setPosition(int modelIndex, glm::vec3 _pos)
{
    this->rigidBody.setPosition(_pos);
    this->geometryRenderer.setModelMatrix(modelIndex, _pos);
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
    this->rigidBody.setPosition(this->rigidBody.getPosition() + translation);
}

void Entity::rotate(float amount, glm::vec3 axis)
{
    this->geometryRenderer.rotate(amount, axis);
    this->rigidBody.setRotation(this->rigidBody.getRotation(axis) + amount, axis);
}

void Entity::scale(glm::vec3 scale)
{
    // this may require manually updating the collision geometry size
    // but I don't expect to have an entity being scaled multiple times
    // beyond when they are first loaded in our game world
    this->geometryRenderer.scale(scale);
}

RigidBody Entity::getRigidBody()
{
    return this->rigidBody;
}

glm::vec3 Entity::getPosition() {
    return rigidBody.getPosition();
}

void Entity::setPosition(glm::vec3 position) {
    rigidBody.updatePosition(position);
    this->geometryRenderer.setModelMatrix(0, position);
}
