#define _USE_MATH_DEFINES // Needed for M_PI

#include "entity.hpp"
#include "pathfinder.hpp"  //for astar stuff
#include "coord.hpp"

Entity::Entity() : meshType(Model::MeshType::BALL), geometryRenderer(Model::meshRenderers[Model::MeshType::BALL]) {}

Entity::Entity(Model::MeshType geometry) : meshType(geometry), geometryRenderer(Model::meshRenderers[geometry]) {}

Entity::~Entity() = default;

//example of using the animate function when overriding Entity
void Entity::animate(float ms) {
	attackingCooldown -= ms;
	if (rigidBody.getAllCollisions().empty()) {
		translate(rigidBody.getVelocity() * ms);
	} else {
		CollisionDetection::CollisionInfo collision = rigidBody.getFirstCollision();
		translate(rigidBody.getVelocity() * collision.time);
	}
}

void Entity::softDelete() {
	geometryRenderer.removeSelf();
	// TODO: AI Comp, Unit Comp soft deletes
}

void Entity::translate(int modelIndex, glm::vec3 translation) {
	// when an entity is translated, translate both the geometry being rendered
	// and the (invisible) collision geometry with it
	this->geometryRenderer.translate(modelIndex, translation);
	this->rigidBody.setPosition(this->rigidBody.getPosition() + translation);
}

void Entity::rotate(int modelIndex, float amount, glm::vec3 axis) {
	// when an entity is rotated, rotate both the geometry being rendered
	// and the (invisible) collision geometry with it
	this->geometryRenderer.rotate(modelIndex, axis);
	this->rigidBody.setRotation(this->rigidBody.getRotation(axis) + amount, axis);
}

void Entity::scale(int modelIndex, glm::vec3 scale) {
	// this may require manually updating the collision geometry size
	// but I don't expect to have an entity being scaled multiple times
	// beyond when they are first loaded in our game world
	this->geometryRenderer.scale(modelIndex, scale);

}

void Entity::setPosition(int modelIndex, glm::vec3 _pos) {
	this->rigidBody.setPosition(_pos);
	this->geometryRenderer.setModelMatrix(modelIndex, _pos);
}


glm::mat4 Entity::getModelMatrix(int modelIndex) const {
	return geometryRenderer.parent->instances[geometryRenderer.id].matrixStack[modelIndex];
}


void Entity::setModelMatrix(int modelIndex, glm::mat4 mat) {
	this->geometryRenderer.setModelMatrix(modelIndex, mat);
}

void
Entity::setModelMatrix(int modelIndex, glm::vec3 translation, float angle, glm::vec3 rotationAxis, glm::vec3 scale) {
	this->geometryRenderer.setModelMatrix(modelIndex, translation, angle, rotationAxis, scale);
}

void Entity::translate(glm::vec3 translation) {
	this->geometryRenderer.translate(translation);
	this->rigidBody.setPosition(this->rigidBody.getPosition() + translation);
}

void Entity::rotate(float amount, glm::vec3 axis) {
	this->geometryRenderer.rotate(amount, axis);
	this->rigidBody.setRotation(this->rigidBody.getRotation(axis) + amount, axis);
}

void Entity::rotateXZ(float amount) {
	angle += amount;
	rotate(amount, {0.0f, 1.0f, 0.0f});
}

void Entity::setRotationXZ(float amount) {
	rotate(amount - angle, {0.0f, 1.0f, 0.0f});
	angle = amount;
}

void Entity::setRotationXZ(int modelIndex, float amount) {
	//rotate(modelIndex, amount - angle, { 0.0f, 1.0f, 0.0f });
	setModelMatrix(modelIndex, glm::rotate(glm::mat4(1.0f), amount, {0.0f, 1.0f, 0.0f}));
	angle = amount;

}

void Entity::scale(glm::vec3 scale) {
	// this may require manually updating the collision geometry size
	// but I don't expect to have an entity being scaled multiple times
	// beyond when they are first loaded in our game world
	this->geometryRenderer.scale(scale);
}

RigidBody Entity::getRigidBody() {
	return this->rigidBody;
}

glm::vec3 Entity::getPosition() {
	return rigidBody.getPosition();
}


Coord Entity::getPositionInt() {
	return {int(rigidBody.getPosition().x + 0.5), int(rigidBody.getPosition().z + 0.5)};
}

void Entity::setPosition(glm::vec3 position) {
	rigidBody.setPosition(position);
	this->geometryRenderer.setModelMatrix(0, position);
}

//only sets unit to its new position (in world coords), everything else is preserved
void Entity::setPositionFast(int modelIndex, glm::vec3 position) {
	rigidBody.setPosition(position);

	glm::mat4 m = getModelMatrix(modelIndex);
	m[3][0] = position.x;
	m[3][1] = position.y;
	m[3][2] = position.z;

	geometryRenderer.setModelMatrix(modelIndex, m);
}

void Entity::setTargetPath(const std::vector<Coord>& targetPath, int x, int z) {
	unitComp.targetPathStartTimestamp = 0;
	unitComp.targetPath = targetPath;
	unitComp.targetDest = Coord(x, z);
}

//expects the caller to set the unit state before calling this
void Entity::moveTo(UnitState unitState, int x, int z) {
	this->unitComp.state = unitState;

	setTargetPath(AI::aStar::findPath(1, this->getPositionInt().colCoord, this->getPositionInt().rowCoord, x,
									  z).second, x, z); //might need fixing with respect to int start positions

	unitComp.targetPath.insert(unitComp.targetPath.begin(), {getPositionInt().colCoord, getPositionInt().rowCoord});
}

//returns a pathIndex and a 0.00 - 0.99 value to interpolate between steps in a path
std::pair<int, double> Entity::getInterpolationPercentage() {
	double intermediateVal = (unitComp.targetPathStartTimestamp / 1000) * unitComp.movementSpeed;
	int pathIndex = (int) intermediateVal;
	double interpolationPercent = clamp<double>(0, intermediateVal - pathIndex, 1);
	return {pathIndex, interpolationPercent};
}

//returns true if this entity can move on the next update
bool Entity::hasMoveTarget() {
	return !this->unitComp.targetPath.empty();
}

void Entity::move(double elapsed_time) {
	if (!hasMoveTarget()) {
		return;
	}

	unitComp.targetPathStartTimestamp += elapsed_time;
	std::pair<int, double> index = getInterpolationPercentage(); //first is index into path, second is interp amount (0 to 1)
	glm::vec3 newPos;
	if (index.first < (int) unitComp.targetPath.size() - 1) {
		Coord curr = unitComp.targetPath[index.first];
		Coord next = unitComp.targetPath[index.first + 1];

		double dRow = next.rowCoord - curr.rowCoord;
		double dCol = next.colCoord - curr.colCoord;

		// TODO: Calculate future velocity for collisions

		double destCol = curr.colCoord + (dCol * index.second);
		double destRow = curr.rowCoord + (dRow * index.second);

		newPos = {destCol, 0, destRow};
	} else { //move to the last coord in the path
		newPos = {unitComp.targetPath.back().colCoord, 0, unitComp.targetPath.back().rowCoord};
		cleanUpTargetPath();
	}

	// TODO: Split this in calculate and update so this can do collisions

	setPositionFast(0, newPos); //for rendering
	rigidBody.setPosition(newPos); //for phys
}

//dont erase targetDest so aimanager can clean up the in progress scouting targets
void Entity::cleanUpTargetPath() {
	unitComp.targetPath.clear();
	unitComp.state = UnitState::IDLE;
}

glm::vec3 Entity::getPosition() const {
	return rigidBody.getPosition();
}

bool Entity::canSee(const Entity& entity) const {
	return glm::length(glm::vec2(entity.getPosition() - this->getPosition())) <= aiComp.visionRange;
}

bool Entity::inAttackRange(const Entity& entity) const {
	return glm::length(glm::vec2(entity.getPosition() - this->getPosition())) <= unitComp.attackRange;
}

bool Entity::operator==(const Entity& rhs) const {
	return geometryRenderer == rhs.geometryRenderer &&
		   aiComp == rhs.aiComp &&
		   unitComp == rhs.unitComp &&
		   rigidBody == rhs.rigidBody;
}

void Entity::takeAttack(const Entity& attackingEntity, double elapsed_ms) {
	// reduce health
	int damagePerSecond = attackingEntity.unitComp.attackDamage * attackingEntity.unitComp.attackSpeed;
	float damageToDoThisFrame = damagePerSecond * (elapsed_ms / 1000);

	aiComp.currentHealth -= damageToDoThisFrame;
}

void Entity::attack(const Entity& entityToAttack) {
	if (unitComp.state == UnitState::ATTACK) {
		// Already attacking something else, nothing to do, return.
		return;
	}

	if (aiComp.type != GamePieceClass::UNIT_OFFENSIVE) return;

	unitComp.state = UnitState::ATTACK;

	// Check to see if attack is done.
	// Set state to non-attacking state if attack is done (other entity is killed)
	if (entityToAttack.aiComp.currentHealth <= 0) {
		unitComp.state = UnitState::IDLE;
	}
}

float vectorAngleXZ(glm::vec3 v) {
	// https://stackoverflow.com/questions/6247153/angle-from-2d-unit-vector
	//if (v.x == 0) {
	//	if (v.z > 0)
	//		return 90.0f;
	//	if (v.z == 0)
	//		return 0.0f;
	//	return 270.0f;
	//}
	//if (v.z == 0) {
	//	if (v.x >= 0)
	//		return 0.0f;
	//	return 180.0f;
	//}
	//float angle = atanf(v.z / v.x)*(180.0f/ M_PI);
	//if (v.x < 0 && v.z < 0)
	//	return 180.0f + angle;
	//if (v.x < 0)
	//	return 180.0f + angle;
	//if (v.z < 0)
	//	return 360.0f + angle;
	glm::vec3 xUnit = glm::vec3(1.0f, 0.0f, 0.0f);
	v = glm::normalize(v);
	return glm::acos(glm::dot(v, xUnit)) * (180.0f / M_PI);
}


void PivotingGunEntity::animate(float ms) {
	attackingCooldown -= ms;
	if (unitComp.currentEnergyLevel <= 0)softDelete();
	// Face the turret to the entity we're attacking
	if (target) { // http://www.cplusplus.com/reference/memory/shared_ptr/operator%20bool/
		targetPosition = target->getPosition();
	}
	glm::vec3 dir = glm::normalize(targetPosition - getPosition());
	float turretAngle = vectorAngleXZ(dir);
	float turretAngle2 = std::atan2(dir.z, dir.x);
	float angleInDegrees = 180 - turretAngle2 * 180.0f / M_PI;
	glm::vec3 xUnit = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 crossProduct = glm::cross(dir, xUnit);
	float crossProductDegrees = crossProduct.y * 180.0f / M_PI;
	setRotationXZ(turretIndex, turretAngle);
}
