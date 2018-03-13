#include "rigidBody.hpp"
#include "collisiondetection.hpp"
#include "model.hpp"

RigidBody::RigidBody(glm::vec3 position, glm::vec3 size, glm::vec3 velocity)
{
    geometryId = Model::collisionDetector.createBoundingBox(position, size, velocity);
}

void RigidBody::updateVelocity(glm::vec3 _velocity)
{
    this->velocity = _velocity;
    Model::collisionDetector.setVelocity(geometryId, _velocity);
}

void RigidBody::setGravity(glm::vec3 _gravity)
{
    this->gravity = _gravity;
}

void RigidBody::updateForce(glm::vec3 _force)
{
    this->appliedForce = _force;
}

void RigidBody::updateOrientation(glm::vec3 _rotation)
{
    this->rotation += _rotation;
    // TODO: Cant currently rotate bounding boxes
}

void RigidBody::updatePosition(glm::vec3 _pos)
{
    this->position = _pos;
    Model::collisionDetector.setPosition(geometryId, _pos);
}

glm::vec3 RigidBody::getPosition()
{
    return position;
}

void RigidBody::setCollisionGeometryType(CollisionGeomType _cg)
{
    this->cgType = _cg; // Todo: doesnt actually affect bounding box
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
