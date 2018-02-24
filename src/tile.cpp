﻿// Header
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

bool Tile::init(const OBJ::Data& obj) {
    return OBJRenderable::init(obj); // Like calling super
}

void Tile::destroy()
{
    OBJRenderable::destroy();
}

void Tile::update(float ms)
{
	// Do nothing, our buildings be static
}

void Tile::setCost(float value)
{
	this->cost = value;
}

float Tile::getCost()
{
	return this->cost;
}
