#include "collisiondetection.hpp"

#include <algorithm>
#include <array>

namespace CollisionDetection {
    CollisionInfo CollisionDetection::movingBodyCollidesWithStatic(BoundingBox staticBox, BoundingBox movingBox, glm::vec3 staticPosition, glm::vec3 movingStartPosition, glm::vec3 movingEndPosition, float totalTime)
    {
        return aabbMinkowskiCollisions(staticBox, movingBox, staticPosition, staticPosition, movingStartPosition, movingEndPosition, totalTime);        
    }

    CollisionInfo aabbMinkowskiCollisions(BoundingBox a, BoundingBox b, glm::vec3 aStart, glm::vec3 aEnd, glm::vec3 bStart, glm::vec3 bEnd, float totalTime)
    {
        // Based off https://gamedev.stackexchange.com/questions/93035/whats-the-fastest-way-checking-if-two-moving-aabbs-intersect#93096
        // TODO: are their coordinates relative or not? this may affect results, but at least wont be subtle

        BoundingBox rotatedA = rotateBoundingBoxAboutOrigin(a);
        BoundingBox sum = minkowskiSum(rotatedA, b);
        
        glm::vec3 aVelocity = (aEnd - aStart) / totalTime;
        glm::vec3 bVelocity = (bEnd - bStart) / totalTime;

        glm::vec3 relativeVelocity = aVelocity - bVelocity;
        // using velocity as a vertex to then check it against the minkowski sum to see if there is a collision
        LineSegment segment = originToVertex(relativeVelocity); 

        return segmentAABBCollision(segment, sum, totalTime);
    }

    CollisionInfo aabbMinkowskiCollisions(CollideableInstance a, CollideableInstance b, float totalTime)
    {
        return aabbMinkowskiCollisions(a.box, b.box, a.start, a.end, b.start, b.end, totalTime);
    }

    BoundingBox normalizeBoundingBox(BoundingBox box)
    {
        BoundingBox result;
        result.upperCorner.x = std::max(box.upperCorner.x, box.lowerCorner.x);
        result.upperCorner.z = std::max(box.upperCorner.z, box.lowerCorner.z);
        result.lowerCorner.x = std::min(box.upperCorner.x, box.lowerCorner.x);
        result.lowerCorner.z = std::min(box.upperCorner.z, box.lowerCorner.z);
        return result;
    }

    BoundingBox rotateBoundingBoxAboutOrigin(BoundingBox box)
    {
        BoundingBox result;
        result.upperCorner.x = -box.upperCorner.x;
        result.upperCorner.z = -box.upperCorner.z;
        result.lowerCorner.x = -box.lowerCorner.x;
        result.lowerCorner.z = -box.lowerCorner.z;
        return normalizeBoundingBox(result);
    }

    BoundingBox minkowskiSum(BoundingBox a, BoundingBox b)
    {
        BoundingBox result;
        result.lowerCorner.x = a.lowerCorner.x + b.lowerCorner.x;
        result.lowerCorner.z = a.lowerCorner.z + b.lowerCorner.z;
        result.upperCorner.x = a.upperCorner.x + b.upperCorner.x;
        result.upperCorner.z = a.upperCorner.z + b.upperCorner.z;
        return result;
    }


    LineSegment originToVertex(glm::vec3 vertex)
    {
        // This is some rocket-science-heart-surgery level of complicated
        return {
            {0,0,0},
            vertex
        };
    }

    IntersectionCollisionInfo segmentsCollisionLeMothe(LineSegment a, LineSegment b)
    {
        // Based off https://stackoverflow.com/a/1968345/848179
        LineSegment segment;
        segment.start.x = a.end.x - a.start.x;
        segment.start.z = a.end.z - a.start.z;
        segment.end.x = b.end.x - b.start.x;
        segment.end.z = b.end.z - b.start.z;

        float s = (-segment.start.z * (a.start.x - b.start.x) + segment.start.x * (a.start.z - b.start.z));
        float t = (segment.end.x * (a.start.z - a.end.z) - segment.end.z * (a.start.x - b.start.x));

        if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
            // Collision happened!
            t = t / (-segment.end.x*segment.start.z + segment.start.x*segment.end.z);
            return {
                true,
                t,
                {a.start.x+t*segment.start.x, 0, a.start.z+t*segment.start.z}
            };
        }
        return {
            false, 0.0f, {0.0f,0.0f,0.0f}
        };
    }

    CollisionInfo segmentAABBCollision(LineSegment segment, BoundingBox box, float totalTime)
    {
        // Effectively we check the segment against all four edges of the box
        float minX, minZ, maxX, maxZ;
        box = normalizeBoundingBox(box); // Just to be safe...
        minX = std::min(box.lowerCorner.x, box.upperCorner.x);
        minZ = std::min(box.lowerCorner.z, box.upperCorner.z);
        maxX = std::max(box.lowerCorner.x, box.upperCorner.x);
        maxZ = std::max(box.lowerCorner.z, box.upperCorner.z);
        glm::vec3 topLeft, bottomRight; // We are given bottom left and top right
        topLeft = { minX, 0, maxZ };
        bottomRight = { maxX, 0, minZ };

        std::vector<LineSegment> boxSides;
        boxSides.push_back({ box.lowerCorner, topLeft });
        boxSides.push_back({ topLeft, box.upperCorner });
        boxSides.push_back({ box.upperCorner, bottomRight });
        boxSides.push_back({ bottomRight, box.lowerCorner });
        
        IntersectionCollisionInfo collision;
        collision.collided = false;
        for (auto side : boxSides) {
            IntersectionCollisionInfo newCollision = segmentsCollisionLeMothe(segment, side);
            if (!collision.collided || newCollision.t < collision.t) {
                collision = newCollision;
            }
        }

        if (collision.collided) {
            // Collision!
            return {
                true,
                collision.t*totalTime
            };
        }
        else {
            return {
                false,
                0
            };
        }
    }

    bool test()
    {
        /*
        Test suite logic follows the following diagram: https://puu.sh/zFbw6/feb4333778.png
        */
        CollideableInstance a;
        a.box.lowerCorner = { 1, 0, 4 };
        a.box.upperCorner = { 2, 0, 5 };
        a.start = { 1.5, 0, 4.5 };
        a.end = { 4.5, 0, 4.5 };

        CollideableInstance b;
        b.box.lowerCorner = { 3, 0, 5.5 };
        b.box.upperCorner = { 4, 0, 6.5 };
        b.start = { 3.5, 0, 6 };
        b.end = { 3.5, 0, 1.5 };

        CollideableInstance c;
        c.box.lowerCorner = { 1.5, 0, 2 };
        c.box.upperCorner = { 2.5, 0, 3 };
        c.start = { 2, 0, 2.5 };
        c.end = { 3.5, 0, 2.5 };

        CollideableInstance d;
        d.box.lowerCorner = { 9,0,4 };
        d.box.upperCorner = { 10,0,5 };
        d.start = { 9.5, 0, 4.5 };
        d.end = { 6.5, 0, 4.5 };

        CollideableInstance e;
        e.box.lowerCorner = { 6, 0, 3 };
        e.box.upperCorner = { 7, 0, 4 };
        e.start = { 6.5, 0, 3.5 };
        e.end = { 9.5, 0, 1.5 };

        CollideableInstance f;
        f.box.lowerCorner = { 6, 0, 1 };
        f.box.upperCorner = { 7, 0, 2 };
        f.start = { 6.5, 0, 1.5 };
        f.end = { 9.5, 0, 3.5 };

        std::array<CollideableInstance, 6> instances = {a,b,c,d,e,f};
        for (int i = 0; i < 6; i++) {
            instances[i].box.upperCorner = { 1, 0, 1 };
            instances[i].box.lowerCorner = { 0,0,0 };
        }
        /*
        Test collisions for A
        */
        for (int i = 0; i < 6; i++) {
            if (i != 0) { // No need to test against self
                CollisionInfo collision = aabbMinkowskiCollisions(instances[0], instances[i], 1.0);
                logger(LogLevel::INFO) << "Collision info [" << i << "]: collided?" << collision.collided << "\n";
            }
        }

        return true;
    }

}