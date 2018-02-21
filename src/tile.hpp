#pragma once

#include "objrenderable.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public OBJRenderable
{
	// shared stuff will go here at some point

public:
	bool init(OBJ::Data obj);
	void destroy();
	void update(float ms);
	void setCost(float);
	float getCost();

private:
	// cost to traverse a tile, used by AI for path finding
	float cost;
};