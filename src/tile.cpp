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

void GunTowerTile::update(float ms)
{
    timeCounter += ms;
    float period = 50;
    glm::vec3 gunDisplacement = { 0, 0, 0.1 };
    float amountToMove = sin(timeCounter / period);
    setModelMatrix(
        2,
        gunDisplacement*amountToMove + glm::vec3({0,0,0.1})
    );
    setModelMatrix(
        3,
        -gunDisplacement*amountToMove + glm::vec3({ 0,0,0.1 })
    );

    rotate(1, ms/1000, { 0,1,0 });
}

void Tile::setCost(float value)
{
	this->cost = value;
}

float Tile::getCost()
{
	return this->cost;
}
