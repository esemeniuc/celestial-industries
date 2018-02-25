#pragma once

#include "objbulkrenderer.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public CompositeObjectBulkRenderable
{
public:
    Tile(std::shared_ptr<CompositeObjectBulkRenderer> _parent) : CompositeObjectBulkRenderable(_parent) {};
	virtual void update(float ms);
private:
	void setCost(float);
	float getCost();
    // cost to traverse a tile, used by AI for path finding
    float cost;
};

class GunTowerTile : public Tile {
private:
    float timeCounter = 0.0f;
public:
    GunTowerTile(std::shared_ptr<CompositeObjectBulkRenderer> _parent) : Tile(_parent) {};
    void update(float ms) override;
};