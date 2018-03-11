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
struct bounding_box {
    //using two corners along the diagonal of box to describe it
    glm::vec3 lowerCorner;
    glm::vec3 upperCorner;
};

struct bounding_sphere {
    double radius;
    glm::vec3 center;
};

enum collision_geometry_type {
    cg_bounding_box,
    cg_bounding_sphere,
};

class RigidBody{
public:
    void setVelocity(glm::vec3);
    void setGravity(glm::vec3);
    void setForce(glm::vec3);
    void setGeometryId(int);
    void setRotation(glm::vec3);
    void setTranslation(glm::vec3);
    void setPosition(glm::vec3);
    glm::vec3 getPosition();
    void setCollisionGeometryType(collision_geometry_type);
    collision_geometry_type getCollisionGeometryType();
    glm::vec3 getVelocity();
    long getGeometryId();

protected:
    // physical properties
    float density            = 1.0f;
    float volume             = 1.0f;
    glm::vec3 velocity       = glm::vec3(0.0f, 0.0f, 0.0f);;
    glm::vec3 gravity        = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 applied_force  = glm::vec3(0.0f, 0.0f, 0.0f);;
    glm::vec3 position       = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation       = glm::vec3(0.0f, 0.0f, 0.0f);

    // collision geometry type
    collision_geometry_type cg_type;

    // id number of the geometry
    int geometry_id;
};
