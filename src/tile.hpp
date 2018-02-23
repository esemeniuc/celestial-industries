#pragma once

#include "objrenderable.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public OBJBulkRenderable
{
public:
    Tile(std::shared_ptr<OBJBulkRenderer> parent) : OBJBulkRenderable::OBJBulkRenderable(parent) {};

	void destroy();

	void update(float ms);

private:
};