#include "collisiondetection.hpp"

#include <algorithm>

namespace CollisionDetection {
    CollisionInfo CollisionDetection::movingBodyCollidesWithStatic(bounding_box staticBox, bounding_box movingBox, glm::vec3 staticPosition, glm::vec3 movingStartPosition, glm::vec3 movingEndPosition, float totalTime)
    {
        // Box is wide in terms of X and high in terms of Z
        float movingWidth = abs(movingBox.upperCorner.x - movingBox.lowerCorner.x);
        float movingHeight = abs(movingBox.upperCorner.z - movingBox.lowerCorner.z);

        
    }

    CollisionInfo aabbMinkowskiCollisions(bounding_box a, bounding_box b, glm::vec3 aStart, glm::vec3 aEnd, glm::vec3 bStart, glm::vec3 bEnd, float totalTime)
    {
        // Based off https://gamedev.stackexchange.com/questions/93035/whats-the-fastest-way-checking-if-two-moving-aabbs-intersect#93096
        // TODO: are their coordinates relative or not? this may affect results, but at least wont be subtle

        bounding_box rotatedA = rotateBoundingBoxAboutOrigin(a);
        bounding_box sum = minkowskiSum(rotatedA, b);
        
        glm::vec3 aVelocity = (aEnd - aStart) / totalTime;
        glm::vec3 bVelocity = (bEnd - bStart) / totalTime;

        glm::vec3 relativeVelocity = aVelocity - bVelocity;
        // using velocity as a vertex to then check it against the minkowski sum to see if there is a collision
        LineSegment segment = originToVertex(relativeVelocity); 

        return segmentAABBCollision(segment, sum, totalTime);
    }

    bounding_box normalizeBoundingBox(bounding_box box)
    {
        bounding_box result;
        result.upperCorner.x = std::max(box.upperCorner.x, box.lowerCorner.x);
        result.upperCorner.z = std::max(box.upperCorner.z, box.lowerCorner.z);
        result.lowerCorner.x = std::min(box.upperCorner.x, box.lowerCorner.x);
        result.lowerCorner.z = std::min(box.upperCorner.z, box.lowerCorner.z);
        return result;
    }

    bounding_box rotateBoundingBoxAboutOrigin(bounding_box box)
    {
        bounding_box result;
        result.upperCorner.x = -box.upperCorner.x;
        result.upperCorner.z = -box.upperCorner.z;
        result.lowerCorner.x = -box.lowerCorner.x;
        result.lowerCorner.z = -box.lowerCorner.z;
        return normalizeBoundingBox(result);
    }

    bounding_box minkowskiSum(bounding_box a, bounding_box b)
    {
        bounding_box result;
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

    CollisionInfo segmentAABBCollision(LineSegment segment, bounding_box box, float totalTime)
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

}