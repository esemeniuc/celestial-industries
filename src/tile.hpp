#pragma once

#include "renderer.hpp"
#include "model.hpp"
#include "entity.hpp"
#include "particle.hpp"

#include <random>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public Entity
{
public:
	glm::vec3 position;
	glm::vec3 size = { 1, 0 ,1 };
	Tile(Model::MeshType mesh);
	Model::MeshType type;
	void update(double ms);
	void moveTo(UnitState unitState, const glm::vec3& moveToTarget, bool queueMove=false) override; // Buildings dont path to places
	virtual void removeSelf();
private:
	//void setCost(float);
	//float getCost();
 //   // cost to traverse a tile, used by AI for path finding
 //   float cost;
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
    void move(double ms) override;
    void explode(glm::vec3 dir);

	void animate(float ms) override;
	void attack(const std::shared_ptr<Entity>& entityToAttack, double elapsed_ms) override;
};

class RefineryTile : public Tile {
public:
	unsigned int resourceCollectionRate;
	void removeSelf() override;
	RefineryTile(unsigned int numberOfGeysersReplaced);
};

class GeyserTile : public Tile {
private:
	std::shared_ptr<Particles::ParticleEmitter> emitter;
public:
	GeyserTile(std::shared_ptr<Shader> particleShader, std::shared_ptr<Texture> particleTexture);
	void setPosition(glm::vec3 position) override;
	void removeSelf() override;
};
