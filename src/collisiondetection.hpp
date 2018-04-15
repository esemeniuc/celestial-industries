#pragma once
#include "glm/mat4x4.hpp"

#include "common.hpp"

// collision geometries
struct BoundingBox {
    //using two corners along the diagonal of box to describe it
    glm::vec3 lowerCorner;
    glm::vec3 upperCorner;
};

namespace CollisionDetection {
    struct MovingBoundingBox {
		bool removed = false;
        BoundingBox box;
        glm::vec3 velocity;
        glm::vec3 position;
    };

    struct CollisionInfo {
        bool collided;
        float time;
		glm::vec3 otherPos;
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
    CollisionInfo movingBodyCollidesWithStatic(BoundingBox staticBox, BoundingBox movingBox, glm::vec3 staticPosition, glm::vec3 movingStartPosition, glm::vec3 movingEndPosition, float totalTime);

    /*
    Based off https://gamedev.stackexchange.com/questions/93035/whats-the-fastest-way-checking-if-two-moving-aabbs-intersect#93096
    */
    CollisionInfo aabbMinkowskiCollisions(BoundingBox a, BoundingBox b, glm::vec3 aStart, glm::vec3 aEnd, glm::vec3 bStart, glm::vec3 bEnd, float totalTime);

    CollisionInfo aabbMinkowskiCollisions(MovingBoundingBox a, MovingBoundingBox b, float totalTime);

    bool aabbsOverlap(BoundingBox a, BoundingBox b);

	bool aabbsOverlap(MovingBoundingBox a, MovingBoundingBox b);

    BoundingBox normalizeBoundingBox(BoundingBox box);

    BoundingBox rotateBoundingBoxAboutOrigin(BoundingBox box);

    BoundingBox minkowskiDiff(BoundingBox a, BoundingBox b);

    BoundingBox minkowskiSum(BoundingBox a, BoundingBox b);

    LineSegment originToVertex(glm::vec3 vertex);

    /*
    Based off https://stackoverflow.com/a/1968345/848179
    */
    IntersectionCollisionInfo segmentsCollisionLeMothe(LineSegment a, LineSegment b);

    /*
    Based off https://stackoverflow.com/questions/3746274/line-intersection-with-aabb-rectangle
    */
    CollisionInfo segmentAABBCollision(LineSegment segment, BoundingBox box, float totalTime);

    // TODO: Move to fancy CI tests at some point
    bool test();
}
