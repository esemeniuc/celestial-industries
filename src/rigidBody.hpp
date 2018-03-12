#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <cstring>

// glm
#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/quaternion.hpp"

// collision geometries
struct BoundingBox {
	//using two corners along the diagonal of box to describe it
	glm::vec3 lowerCorner;
	glm::vec3 upperCorner;
};

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
	void setVelocity(glm::vec3);

	void setGravity(glm::vec3);

	void setForce(glm::vec3);

	void setGeometryId(int);

	void setRotation(float, glm::vec3);

	void setPosition(glm::vec3);

	glm::vec3 getPosition();

    glm::vec3 getRotation();

	void setCollisionGeometryType(CollisionGeomType);

	CollisionGeomType getCollisionGeometryType();

	glm::vec3 getVelocity();

	long getGeometryId();

protected:
	// physical properties
	float density = 1.0f;
	float volume = 1.0f;
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
