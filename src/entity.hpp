#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <string.h>

// glm
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// collision geometries
struct bounding_box {
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

enum collision_geometry {
	cg_bounding_box,
	cg_bounding_sphere,
	cg_bonding_cylinder,
};

class Entity {
public:
	void set_velocity (glm::vec3 &);
	void set_gravity (glm::vec3 &);
	void set_force (glm::vec3 &);
	glm::vec3 get_velocity();
	void set_collision_geometry_position (glm::vec3 &);
	

private:
	// phyisical properties
	double density;
	double volume;
	glm::vec3 velocity;
	glm::vec3 applied_force;
	glm::vec3 gravity;

	// collision geometry type
	collision_geometry cg;

	// graphics attributes
	bool isTextured;
	






};