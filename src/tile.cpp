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

Tile::Tile(Model::MeshType geometry) : geometryRenderer(Model::meshRenderers[geometry]) {}

Tile::~Tile() = default;

void Tile::update(float ms)
{
    // Do nothing
}

void Tile::translate(glm::vec3 v)
{
	position += v;
	geometryRenderer.translate(v);
}

void Tile::setPosition(glm::vec3 v)
{
	position = v;
	geometryRenderer.setModelMatrix(0, glm::translate(glm::mat4(1.0f), v));
}

GunTowerTile::GunTowerTile() : Tile(Model::MeshType::GUN_TURRET)
{
     randomDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
}

void GunTowerTile::update(float ms)
{
    timeCounter += ms;
    if (exploding) {
        for (size_t i = 1; i < explosionDirections.size(); i++) {
			geometryRenderer.translate(i, explosionDirections[i] * (ms*explosionVelocity / 1000.0f), false);
            /*rotate(i, explosionVelocity*ms/1000, explosionRotationalAxes[i], false);*/
        }
    }
    else {
        float period = 50;
        glm::vec3 gunDisplacement = { 0.0, 0.0, 0.1 };
        float amountToMove = sin(timeCounter / period);
		geometryRenderer.setModelMatrix(
            2,
            gunDisplacement*amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
        );
		geometryRenderer.setModelMatrix(
            3,
            -gunDisplacement * amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
        );

		geometryRenderer.rotate(1, ms / 1000, { 0,1,0 });
    }
}

void GunTowerTile::explode(glm::vec3 dir)
{
    exploding = true;
    explosionVelocity = randomDistribution(randomEngine) * 10.0f - 5.0f;
	geometryRenderer.setModelMatricesFromComputed();
    for (size_t i = 0; i < geometryRenderer.parent->subObjects.size(); i++) {
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

void Tile::softDelete()
{
	geometryRenderer.removeSelf();
}

void Tile::setCost(float value)
{
	this->cost = value;
}

float Tile::getCost()
{
	return this->cost;
}
