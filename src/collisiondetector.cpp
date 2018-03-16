#include "collisiondetector.hpp"
#include "collisiondetection.hpp"
#include "rigidBody.hpp"

#include <map>


int CollisionDetector::createBoundingBox(glm::vec3 position, glm::vec3 size, glm::vec3 velocity)
{
    CollisionDetection::MovingBoundingBox result;
    result.box.lowerCorner = position - size / 2.0f;
    result.box.upperCorner = position + size / 2.0f;
    result.velocity = velocity;
    int id = boxes.size();
    boxes.push_back(result);
    collisions.push_back({});
    return id;
}

void CollisionDetector::findCollisions(float elapsed_ms)
{
    // O(n^2) lol
    for (int i = 0; i < boxes.size(); i++) {
        collisions[i].clear();
        for (int j = 0; j < boxes.size(); j++) {
            if (i != j) {
                CollisionDetection::CollisionInfo collision = CollisionDetection::aabbMinkowskiCollisions(boxes[i], boxes[j], elapsed_ms);
                if (collision.collided) {
                    // TODO: Currently only tracks the time of collision, not anything else
                    collisions[i].push_back(collision);
                }
            }
        }
    }
}

std::vector<CollisionDetection::CollisionInfo> CollisionDetector::getAllCollisions(int id)
{
    return collisions[id];
}

CollisionDetection::CollisionInfo CollisionDetector::getFirstCollision(int id)
{
    CollisionDetection::CollisionInfo firstCollision;
    firstCollision.collided = false;
    for (int i = 0; i < collisions[id].size(); i++) {
        if (!firstCollision.collided || (collisions[id][i].collided && collisions[id][i].time < firstCollision.time)) {
            firstCollision = collisions[id][i];
        }
    }
    return firstCollision;
}

void CollisionDetector::setVelocity(int id, glm::vec3 velocity)
{
    boxes[id].velocity = velocity;
}

void CollisionDetector::setPosition(int id, glm::vec3 position)
{
    boxes[id].position = position;
}
