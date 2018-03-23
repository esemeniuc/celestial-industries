#pragma once
#include "common.hpp"
#include "rigidBody.hpp"
#include "collisiondetection.hpp"

class CollisionDetector {
    std::vector<CollisionDetection::MovingBoundingBox> boxes;
    std::vector<std::vector<CollisionDetection::CollisionInfo>> collisions;
    
public:
    /*
    Creates a collision geometry of type type and adds it to the list of stuff to check collisions for.
    Returns the ID of the box;
    */
    int createBoundingBox(glm::vec3 position, glm::vec3 size, glm::vec3 velocity = { 0,0,0 });

    /*
    Called once per update loop to compute the collisions between all tracked elements
    */
    void findCollisions(float elapsed_ms);

    std::vector<CollisionDetection::CollisionInfo> getAllCollisions(int id);
    CollisionDetection::CollisionInfo getFirstCollision(int id);

    void setVelocity(int id, glm::vec3 velocity);
    void setPosition(int id, glm::vec3 position);

	// Todo: Currently just a soft delete - cant rearrange everything without invalidating IDs
	void remove(int id);

    // TODO add methods to scale and rotate
};
