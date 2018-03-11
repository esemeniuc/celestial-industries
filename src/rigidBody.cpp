#include "rigidBody.hpp"

void RigidBody::setVelocity(glm::vec3 _velocity)
{
    this->velocity = _velocity;
}

void RigidBody::setGravity(glm::vec3 _gravity)
{
    this->gravity = _gravity;
}

void RigidBody::setForce(glm::vec3 _force)
{
    this->applied_force = _force;
}

void RigidBody::setGeometryId(int _id)
{
    this->geometry_id = _id;
}

void RigidBody::setRotation(glm::vec3 _rotation)
{
    this->rotation = _rotation;
}

void RigidBody::setTranslation(glm::vec3 _translation)
{
    this->position += _translation;
}

void RigidBody::setPosition(glm::vec3 _pos)
{
    this->position = _pos;
}

glm::vec3 RigidBody::getPosition()
{
    return position;
}

void RigidBody::setCollisionGeometryType(collision_geometry_type _cg)
{
    this->cg_type = _cg;
}

collision_geometry_type RigidBody::getCollisionGeometryType()
{
    return this->cg_type;
}

glm::vec3 RigidBody::getVelocity()
{
    return this->velocity;
}

long RigidBody::getGeometryId()
{
    return this->geometry_id;
}
