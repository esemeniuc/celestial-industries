#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <cstring>
#include <map>

// glm
#include "glm/glm.hpp"

// custom headers
#include "objloader.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "rigidBody.hpp"

enum class EntityOwner {
	NONE, PLAYER, AI
};

enum class EntityType {
	NONE, NON_ATTACKING, DEFENSIVE_PASSIVE, DEFENSIVE_ACTIVE, OFFENSIVE
};

struct EntityInfo {
	EntityOwner owner = EntityOwner::NONE;
	EntityType type = EntityType::NONE;
	int value; //used for AI to prioritize targets
};

class Entity {
public:
	//members
	EntityInfo aiInfo;

	// constructors
	Entity();

	Entity(Model::MeshType geometry);

	// functions
	virtual void animate(float ms);

	void translate(int modelIndex, glm::vec3 translation);

	void rotate(int modelIndex, float amount, glm::vec3 axis);

	void scale(int modelIndex, glm::vec3 scale);

	void setModelMatrix(int modelIndex, glm::mat4 mat);

	void setModelMatrix(int modelIndex, glm::vec3 translation = {0, 0, 0}, float angle = 0,
						glm::vec3 rotationAxis = {0, 1, 0}, glm::vec3 scale = {1, 1, 1});

	// When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
	void translate(glm::vec3 translation);

	void rotate(float amount, glm::vec3 axis);

	void scale(glm::vec3 scale);

protected:
	Renderable geometryRenderer;
    RigidBody rigidBody;
};
