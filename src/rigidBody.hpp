#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <cstring>
#include "config.hpp"
#include "collisiondetection.hpp"


// glm
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

struct BoundingSphere {
	double radius;
	glm::vec3 center;
};

enum class CollisionGeomType {
	cgBoundingBox,
	cgBoundingSphere,
};

class RigidBody {
public:
	RigidBody(glm::vec3 _position = {0, 0, 0}, glm::vec3 _size = {1, 0, 1}, glm::vec3 _velocity = {0, 0, 0});

	void setVelocity(glm::vec3);

	void setGravity(glm::vec3);

	void setForce(glm::vec3);

	void setGeometryId(int);

	void setRotation(float, glm::vec3);

	void setPosition(glm::vec3);

	void setInverseMass(float);

	void setMass(float);

	void setDampingFactor(float);

	void setCollisionGeometryType(CollisionGeomType);

	glm::vec3 getPosition() const;

	float getRotation(glm::vec3);

	float getInverseMass();

	float getMass();

	float getDampingFactor();

	glm::vec3 getForce();

	CollisionGeomType getCollisionGeometryType();

	glm::vec3 getVelocity();

	long getGeometryId();

	bool operator==(const RigidBody& rhs) const;

	void removeSelf();

	std::vector<CollisionDetection::CollisionInfo> getAllCollisions();

	CollisionDetection::CollisionInfo getFirstCollision();

protected:
	// this is 1/mass, a better representation that 
	// allows us to work with 0 and infinite masses
	float inverseMass;

	// velocity damping factor used to simulate
	// drag and friction forces in the real world

	float dampingFactor = 0.995f;

	glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 appliedForce = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

	// collision geometry type
	CollisionGeomType cgType;

	// id number of the geometry
	int geometryId;
};
