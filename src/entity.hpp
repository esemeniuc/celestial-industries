#pragma once

#include <cassert>
#include <cmath>
#include <sstream>
#include <cstring>
#include <map>
#include <unit/gameunit.hpp>

// glm
#include "glm/glm.hpp"

// custom headers
#include "objloader.hpp"
#include "renderer.hpp"
#include "model.hpp"
#include "rigidBody.hpp"
#include "gamepiece.hpp"


enum class EntityOwner {
	NONE, PLAYER, AI
};

enum class EntityType {
	NONE, NON_ATTACKING, DEFENSIVE_PASSIVE, DEFENSIVE_ACTIVE, OFFENSIVE
};

class Entity {
public:
	//members
	Renderable geometryRenderer;


	// constructors
	Entity();

	Entity(Model::MeshType geometry);


	// functions
	virtual void animate(float ms);

	// Model index is the index of the model matrix to be updated.This is generally 0 as most models are made of 1 object and only have
	// 1 model matrix.It may also be 0 because the first model matrix is usually the root of a renderable made of many objects.If you want
	// to modify the model matrix of the 3rd element of your renderable(and all it's children in turn) you specify 2 (0 indexing).

	void translate(int modelIndex, glm::vec3 translation);

	glm::vec3 getPosition();

	void setPosition(glm::vec3 position);

	void setPositionFast(int modelIndex, glm::vec3 position);

	void rotate(int modelIndex, float amount, glm::vec3 axis);

	void scale(int modelIndex, glm::vec3 scale);

	void setPosition(int modelIndex, glm::vec3);

	void setModelMatrix(int modelIndex, glm::mat4 mat);

	void setModelMatrix(int modelIndex, glm::vec3 translation = {0, 0, 0}, float angle = 0,
						glm::vec3 rotationAxis = {0, 1, 0}, glm::vec3 scale = {1, 1, 1});

	glm::mat4 getModelMatrix(int modelIndex) const;

	// When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
	void translate(glm::vec3 translation);

	void rotate(float amount, glm::vec3 axis);

	void scale(glm::vec3 scale);

	RigidBody getRigidBody();

	//funcs
	glm::vec3 getPosition() const;

	void setTargetPath(const std::vector<Coord>& targetPath);

	void move(double elapsed_time);

	std::pair<int, double> getInterpolationPercentage();

	bool inVisionRange(const Entity& other);

	bool inAttackRange(const Entity& other);


protected:

	AiComp aiComp;
	UnitComp unitComp;

	RigidBody rigidBody;
};
