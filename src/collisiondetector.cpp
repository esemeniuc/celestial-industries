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
	result.removed = false;
    int id = boxes.size();
    boxes.push_back(result);
    collisions.push_back({});
    return id;
}

void CollisionDetector::findCollisions(float elapsed_ms)
{
    // O(n^2) lol
    for (size_t i = 0; i < boxes.size(); i++) {
        collisions[i].clear();
		if (!boxes[i].removed) {
			for (size_t j = 0; j < boxes.size(); j++) {
				if (i != j && !boxes[j].removed) {
					// Detect moving collisions
					CollisionDetection::CollisionInfo collision = CollisionDetection::aabbMinkowskiCollisions(boxes[i], boxes[j], elapsed_ms);
					if (collision.collided) {
						// TODO: Currently only tracks the time of collision, not anything else
						collision.otherPos = boxes[j].position;
						glm::vec3 distance = boxes[j].position - boxes[i].position;
						//collisions[i].push_back(collision);
					}

					// Static collisions
					if (CollisionDetection::aabbsOverlap(boxes[i], boxes[j])) {
						CollisionDetection::CollisionInfo collision = {
							true,
							0.0f,
							boxes[j].position
						};
						collisions[i].push_back(collision);
					}
				}
			}
		}
    }
	return;
}

std::vector<CollisionDetection::CollisionInfo> CollisionDetector::getAllCollisions(int id)
{
    return collisions[id];
}

CollisionDetection::CollisionInfo CollisionDetector::getFirstCollision(int id)
{
    CollisionDetection::CollisionInfo firstCollision;
    firstCollision.collided = false;
    for (size_t i = 0; i < collisions[id].size(); i++) {
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

void CollisionDetector::remove(int id)
{
	boxes[id].removed = true;
}
