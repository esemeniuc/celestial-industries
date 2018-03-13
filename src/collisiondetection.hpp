#pragma once
#include "common.hpp"

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

class RigidBody {
public:
    void updateVelocity(glm::vec3);
    void setGravity(glm::vec3);
    void updateForce(glm::vec3);
    void setGeometryId(int);
    void updateOrientation(glm::vec3);
    void updatePosition(glm::vec3);
    glm::vec3 getPosition();
    void setCollisionGeometryType(collision_geometry_type);
    collision_geometry_type getCollisionGeometryType();
    glm::vec3 getVelocity();
    long getGeometryId();

protected:
    // physical properties
    float density = 1.0f;
    float volume = 1.0f;
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);;
    glm::vec3 gravity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 applied_force = glm::vec3(0.0f, 0.0f, 0.0f);;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    // collision geometry type
    collision_geometry_type cg_type;

    // id number of the geometry
    int geometry_id;
};

namespace CollisionDetection {
    struct CollisionInfo {
        bool collided;
        float time;
    };

    struct IntersectionCollisionInfo {
        bool collided;
        float t;
        glm::vec3 intersection;
    };

    struct LineSegment {
        glm::vec3 start, end;
    };
    
    /**
    Returns a bool and a float between 0 and totalTime. If the boolean returned is true then there was a collision,
    and the float represents the time at which the collision occurs (between 0 and totalTime)
    */
    CollisionInfo movingBodyCollidesWithStatic(bounding_box staticBox, bounding_box movingBox, glm::vec3 staticPosition, glm::vec3 movingStartPosition, glm::vec3 movingEndPosition, float totalTime);

    /*
    Based off https://gamedev.stackexchange.com/questions/93035/whats-the-fastest-way-checking-if-two-moving-aabbs-intersect#93096
    */
    CollisionInfo aabbMinkowskiCollisions(bounding_box a, bounding_box b, glm::vec3 aStart, glm::vec3 aEnd, glm::vec3 bStart, glm::vec3 bEnd, float totalTime);

    bounding_box normalizeBoundingBox(bounding_box box);

    bounding_box rotateBoundingBoxAboutOrigin(bounding_box box);

    bounding_box minkowskiSum(bounding_box a, bounding_box b);

    LineSegment originToVertex(glm::vec3 vertex);

    /*
    Based off https://stackoverflow.com/a/1968345/848179
    */
    IntersectionCollisionInfo segmentsCollisionLeMothe(LineSegment a, LineSegment b);

    /*
    Based off https://stackoverflow.com/questions/3746274/line-intersection-with-aabb-rectangle
    */
    CollisionInfo segmentAABBCollision(LineSegment segment, bounding_box box, float totalTime);
}