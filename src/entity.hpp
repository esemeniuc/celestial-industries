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
#include "gamepiece.hpp"

class Entity {
public:
	// constructors
	Entity();

	Entity(Model::MeshType geometry);

	// functions
	virtual void animate(float ms);

	void translate(int modelIndex, glm::vec3 translation);

	void rotate(int modelIndex, float amount, glm::vec3 axis);

	void scale(int modelIndex, glm::vec3 scale);

	void setModelMatrix(int modelIndex, glm::mat4 mat);

	glm::mat4 getModelMatrix(int modelIndex) const;

	void setModelMatrix(int modelIndex, glm::vec3 translation = {0, 0, 0}, float angle = 0,
						glm::vec3 rotationAxis = {0, 1, 0}, glm::vec3 scale = {1, 1, 1});

	// When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
	void translate(glm::vec3 translation);

	void rotate(float amount, glm::vec3 axis);

	void scale(glm::vec3 scale);

	//funcs
	glm::vec3 getPosition() const {
		return rigidBody.getPosition();
	}


protected:

	GamePiece gamePiece;
	RigidBody rigidBody;
	Renderable geometryRenderer;
};
