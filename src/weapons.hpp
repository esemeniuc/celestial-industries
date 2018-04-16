#pragma once
#include "renderer.hpp"
#include "model.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

class Weapon {
public:
	Renderable geometryRenderer;
	double timeSinceStart;
	bool isDeleted;
	Weapon(Model::MeshType type, glm::vec3 start, glm::vec3 end);
	virtual void update(double ms);
	void softDelete();
	glm::vec3 start;
	glm::vec3 end;
};

class BeamWeapon : public Weapon {
public:
	double lifespan;

	BeamWeapon(Model::MeshType type, glm::vec3 start, glm::vec3 end, double lifespan);
	void update(double ms) override;
};

class ProjectileWeapon : public Weapon {

};
