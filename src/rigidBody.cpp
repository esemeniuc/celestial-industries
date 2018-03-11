#include "rigidBody.hpp"

void RigidBody::updateVelocity(glm::vec3 _velocity)
{
    this->velocity = _velocity;
}

void RigidBody::setGravity(glm::vec3 _gravity)
{
    this->gravity = _gravity;
}

void RigidBody::updateForce(glm::vec3 _force)
{
    this->appliedForce = _force;
}

void RigidBody::setGeometryId(int _id)
{
    this->geometryId = _id;
}

void RigidBody::updateOrientation(glm::vec3 _rotation)
{
    this->rotation += _rotation;
}

void RigidBody::updatePosition(glm::vec3 _pos)
{
    this->position = _pos;
}

glm::vec3 RigidBody::getPosition()
{
    return position;
}

void RigidBody::setCollisionGeometryType(CollisionGeomType _cg)
{
    this->cgType = _cg;
}

CollisionGeomType RigidBody::getCollisionGeometryType()
{
    return this->cgType;
}

glm::vec3 RigidBody::getVelocity()
{
    return this->velocity;
}

long RigidBody::getGeometryId()
{
    return this->geometryId;
}
