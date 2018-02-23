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

// custom headers
#include "objrenderable.hpp"
#include "objloader.hpp"

// collision geometries
struct bounding_box {

//               	 @ + + + + + + + + @						|
//               	 +\                +\						|
//               	 + \               + \						|
//               	 +  \              +  \         y			|
//           width   +   \             +   \        |			|
//               	 +    @ + + + + + +++ + @       |______ x	|
//               	 +    +            +    +        \			|
//               	 +    +            +    +         \			|
//               	 +    +            +    +          z		|
//  corner position  @ + +++ + + + + + @    +					|
//               	  \   +             \   +					|
//               	   \  +              \  +					|
//               height \ +               \ +					|
//               	     \+    length      \+					|
//               	      @ + + + + + + + + @					|

	double length, width, height;
	glm::vec3 corner_position;
};

struct bounding_sphere {
	double radius;
	glm::vec3 center;
};

struct bounding_cylinder {
	double radius, height;
	glm::vec3 center;
};

enum collision_geometry_type {
	cg_bounding_box,
	cg_bounding_sphere,
	cg_bounding_cylinder,
};

class Entity : public OBJRenderable {
public:
	virtual void update(float ms) = 0;
	void setvelocity (glm::vec3);	
	void setGravity (glm::vec3);
	void setForce (glm::vec3);
	void setGeometryId(long);
	void setRotation (glm::vec3);
	void setTranslation (glm::vec3);
	void setScale(glm::vec3);
	void setPosition(glm::vec3);
	void setCameraPosition(glm::vec3);
	glm::vec3 getPosition();
	void applyTransformations();
	void setCollisionGeometryType(collision_geometry_type);
	glm::mat4 getModelMatrix();
	collision_geometry_type getCollisionGeometryType();
	glm::vec3 getVelocity();
	long getGeometryId();
	bool isTextured();

protected:
	// physical properties
	double density;
	double volume;
	glm::vec3 velocity;
	glm::vec3 gravity;
	glm::vec3 applied_force;
	glm::vec3 translation = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 scale = glm::vec3(1.0, 1.0, 1.0);
	glm::vec3 rotation = glm::vec3(0.0, 0.0,0.0);
	glm::vec3 position;
	glm::vec3 cameraPosition;
	glm::mat4 model = glm::mat4(1.0);
	
	// collision geometry type
	collision_geometry_type cg_type;

	// graphics attributes
	size_t indices;
	bool texture_flag;

	// id number of the geometry
	long geometry_id;
};