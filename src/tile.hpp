#pragma once

#include "renderer.hpp"
#include "model.hpp"

#include <random>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile
{
public:
    Tile(Model::MeshType geometry);
    virtual ~Tile();
	virtual void update(float ms);
	void translate(glm::vec3 v);
	void setPosition(glm::vec3 v);
	Renderable geometryRenderer;
	glm::vec3 position;
	void softDelete();
private:
	void setCost(float);
	float getCost();
    // cost to traverse a tile, used by AI for path finding
    float cost;
};

class GunTowerTile : public Tile {
private:
    float timeCounter = 0.0f;
    bool exploding = false;
    std::vector<glm::vec3> explosionDirections;
    std::vector<glm::vec3> explosionRotationalAxes;
    float explosionVelocity;


    // TODO: Share RNG across everything?
    std::random_device randomEngine;
    std::uniform_real_distribution<double> randomDistribution; // default 0..1

public:
    GunTowerTile();
    void update(float ms) override;
    void explode(glm::vec3 dir);

};
