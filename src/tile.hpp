#pragma once

#include "objbulkrenderer.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public CompositeObjectBulkRenderable
{
public:
    Tile(std::shared_ptr<CompositeObjectBulkRenderer> parent) : CompositeObjectBulkRenderable(parent) {};
	virtual void update(float ms);
private:
};

class GunTowerTile : public Tile {
private:
    float timeCounter = 0.0f;
public:
    GunTowerTile(std::shared_ptr<CompositeObjectBulkRenderer> parent) : Tile(parent) {};
    void update(float ms) override;
};