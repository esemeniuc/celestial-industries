#pragma once

// glm
#include "glm/glm.hpp"

// custom headers

#include "aicomp.hpp"
#include "model.hpp"
#include "renderer.hpp"
#include "rigidBody.hpp"
#include "unitcomp.hpp"

class Entity {
public:
	//members
	Renderable geometryRenderer;
	AiComp aiComp;
	UnitComp unitComp;
	RigidBody rigidBody;

	std::shared_ptr<Entity> target;
	glm::vec3 targetPosition = {0.0f, 0.0f, 0.0f};
	float attackingCooldown;

	// constructors
	Entity();

	Entity(Model::MeshType geometry);

	virtual ~Entity();
	// functions
	virtual void animate(float ms);

	// Soft deletes the entity (actual data will only be removed once there are no refference to Entity or the game closes or someone calls the destructor
	void softDelete();

	// Model index is the index of the model matrix to be updated.This is generally 0 as most models are made of 1 object and only have
	// 1 model matrix.It may also be 0 because the first model matrix is usually the root of a renderable made of many objects.If you want
	// to modify the model matrix of the 3rd element of your renderable(and all it's children in turn) you specify 2 (0 indexing).

	void translate(int modelIndex, glm::vec3 translation);

	glm::vec3 getPosition();

	Coord getPositionInt();

	void attack(const std::shared_ptr<Entity>& entityToAttack, double elapsed_ms);

	void takeAttack(const Entity& attackingEntity, double elapsed_ms);

	void setPosition(glm::vec3 position);

	void setPositionFast(int modelIndex, glm::vec3 position);

	// Please do not use
	void rotate(int modelIndex, float amount, glm::vec3 axis);

	void scale(int modelIndex, glm::vec3 scale);

	void setPosition(int modelIndex, glm::vec3);

	void setModelMatrix(int modelIndex, glm::mat4 mat);

	void setModelMatrix(int modelIndex, glm::vec3 translation = {0, 0, 0}, float angle = 0,
						glm::vec3 rotationAxis = {0, 1, 0}, glm::vec3 scale = {1, 1, 1});

	glm::mat4 getModelMatrix(int modelIndex) const;

	// When subobject modelIndex is not provided it is assumed you wish to apply the transformation to the whole model
	void translate(glm::vec3 translation);

	// Please do not use
	void rotate(float amount, glm::vec3 axis);

	void rotateXZ(float amount);

	void setRotationXZ(float amount);

	void setRotationXZ(int modelIndex, float amount);

	void scale(glm::vec3 scale);

	RigidBody getRigidBody();

	glm::vec3 getPosition() const;

	void setTargetPath(const std::vector<Coord>& targetPath, int x, int z);

	bool hasMoveTarget();

	void moveTo(UnitState unitState, int x, int z);

	void cleanUpTargetPath();

	void move(double elapsed_time);

	std::pair<int, double> getInterpolationPercentage();

	bool canSee(const std::shared_ptr<Entity>& other) const;

	bool inAttackRange(const std::shared_ptr<Entity>& other) const;

	bool operator==(const Entity& rhs) const;

protected:
	float angle = 0.0f;
};

// TODO: Override rotate methods so that they also update angle
class PivotingGunEntity : public Entity {
public:
	unsigned int turretIndex;
	float turretAngle;

	PivotingGunEntity(Model::MeshType geometry, unsigned int turretIndex) : Entity(geometry), turretIndex(turretIndex) {};

	void animate(float ms) override;
};
