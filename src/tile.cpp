// Header
#include "tile.hpp"
#include "objloader.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>


// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Tile::update(float ms)
{
    // Do nothing
}

GunTowerTile::GunTowerTile(std::shared_ptr<Renderer> _parent) : Tile(_parent)
{
     randomDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
}

void GunTowerTile::update(float ms)
{
    timeCounter += ms;
    if (exploding) {
        for (int i = 1; i < explosionDirections.size(); i++) {
            translate(i, explosionDirections[i] * (ms*explosionVelocity / 1000.0f), false);
            /*rotate(i, explosionVelocity*ms/1000, explosionRotationalAxes[i], false);*/
        }
    }
    else {
        if (timeCounter > 4000) {
            explode({ 0,1,0 });
        }
        else {
            float period = 50;
            glm::vec3 gunDisplacement = { 0.0, 0.0, 0.1 };
            float amountToMove = sin(timeCounter / period);
            setModelMatrix(
                2,
                gunDisplacement*amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
            );
            setModelMatrix(
                3,
                -gunDisplacement * amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
            );

            rotate(1, ms / 1000, { 0,1,0 });
        }
    }
}

void GunTowerTile::explode(glm::vec3 dir)
{
    exploding = true;
    explosionVelocity = randomDistribution(randomEngine) * 10 - 5;
    setModelMatricesFromComputed();
    for (int i = 0; i < parent->subObjects.size(); i++) {
        // Calculate a random direction to send the subobject in and give it a velocity.
        glm::vec3 randomOffset = glm::normalize(glm::vec3(randomDistribution(randomEngine), randomDistribution(randomEngine), randomDistribution(randomEngine)));
        glm::vec3 result = glm::normalize(dir + randomOffset);
        explosionRotationalAxes.push_back(
            glm::normalize(glm::vec3(randomDistribution(randomEngine), randomDistribution(randomEngine), randomDistribution(randomEngine)))
        );
        if (explosionVelocity < 0) {
            if (result.y > 0)
                result.y = -result.y;
        }
        explosionDirections.push_back(result);
    }
    timeCounter = 0.0f;
}

void Tile::setCost(float value)
{
	this->cost = value;
}

float Tile::getCost()
{
	return this->cost;
}
