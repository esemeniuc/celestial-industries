#define _USE_MATH_DEFINES // Needed for M_PI

#include "entity.hpp"
#include "pathfinder.hpp"  //for astar stuff
#include "coord.hpp"
#include "global.hpp"

Entity::Entity() : meshType(Model::MeshType::BALL), geometryRenderer(Model::meshRenderers[Model::MeshType::BALL]) {}

Entity::Entity(Model::MeshType geometry) : meshType(geometry), geometryRenderer(Model::meshRenderers[geometry]) {}

Entity::~Entity() = default;

//example of using the animate function when overriding Entity
void Entity::animate(float ms) {
	// Do nothing
}

void Entity::softDelete() {
	geometryRenderer.removeSelf();
	rigidBody.removeSelf();
	isDeleted = true;
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

void Entity::setRotationXZ(int modelIndex, glm::vec3 dir) {
	glm::mat4 rotationMatrix = glm::orientation(dir, { 0,0, -1 });
	setModelMatrix(modelIndex, rotationMatrix);
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

void Entity::setTargetPath(const std::vector<glm::vec3>& targetPath) {
	unitComp.targetPathStartTimestamp = 0;
	unitComp.targetPath = targetPath;
}

void Entity::moveTo(UnitState unitState, const glm::vec3& moveToTarget, bool queueMove) {
	this->unitComp.state = unitState;
	if (!queueMove) {
		destinations.clear(); // Clear the queue
		cleanUpTargetPath();
	}
	hasDestination = true;
	destinations.push_back(moveToTarget);
}

//returns a pathIndex and a 0.00 - 0.99 value to interpolate between steps in a path
std::pair<int, float> Entity::getInterpolationPercentage() {
	float intermediateVal = (unitComp.targetPathStartTimestamp / 1000) * unitComp.movementSpeed;
	int pathIndex = (int) intermediateVal;
	float interpolationPercent = clamp<float>(0, intermediateVal - pathIndex, 1);
	return {pathIndex, interpolationPercent};
}

//returns true if this entity can move on the next update
bool Entity::hasMoveTarget() {
	return !this->unitComp.targetPath.empty();
}

void Entity::computeNextMoveLocation(double elapsed_time) {
	if (!hasMoveTarget()) {
		return;
	}

	unitComp.targetPathStartTimestamp += elapsed_time;
	std::pair<int, float> index = getInterpolationPercentage(); //first is index into path, second is interp amount (0 to 1)
	if (index.first < (int) unitComp.targetPath.size() - 1) {
		glm::vec3 curr = unitComp.targetPath[index.first];
		glm::vec3 next = unitComp.targetPath[index.first + 1];

		nextPosition = glm::mix(curr, next, index.second);
	} else { //move to the last coord in the path
		nextPosition = unitComp.targetPath.back();
		cleanUpTargetPath();
	}
	rigidBody.setVelocity(nextPosition - rigidBody.getPosition());
}

void Entity::move(double elapsed_time) {
	if (!hasMoveTarget()) {
		if (!destinations.empty()) {
			currentDestination = destinations.front(); //get the next dest
			destinations.pop_front();
			setTargetPath(AI::aStar::findPath(this->getPosition(), currentDestination).second);
		}
		return;
	}

	bool hasCollision = !rigidBody.getAllCollisions().empty();
	if (!hasPhysics || !hasCollision || collisionCooldown > 0) {
		setPositionFast(0, nextPosition); //for rendering
		rigidBody.setPosition(nextPosition); //for phys
		if (collisionCooldown > 0)collisionCooldown -= elapsed_time;
	} else {
		CollisionDetection::CollisionInfo collision = rigidBody.getFirstCollision();
		if (hasDestination) {
			glm::vec3 vecFromOther = getPosition() - collision.otherPos;
			glm::vec3 bounceDir = glm::cross(vecFromOther, {0, 1, 0});
			glm::vec3 destination = getPosition() + vecFromOther;
			destinations.emplace_front(currentDestination);
			destinations.emplace_front(destination);
			currentDestination = destination;
			unitComp.targetPath.clear();
			collisionCooldown = 10.0f;
		}
	}
}

//dont erase targetDest so aimanager can clean up the in progress scouting targets
void Entity::cleanUpTargetPath() {
	unitComp.targetPath.clear();
	unitComp.state = UnitState::IDLE;
}

glm::vec3 Entity::getPosition() const {
	return rigidBody.getPosition();
}

bool Entity::canSee(const std::shared_ptr<Entity>& entity) const {
	return glm::distance(entity->getPosition(), this->getPosition()) <= aiComp.visionRange;
}

bool Entity::inAttackRange(const std::shared_ptr<Entity>& entity) const {
	return glm::distance(entity->getPosition(), this->getPosition()) <= unitComp.attackRange;
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

void Entity::attack(const std::shared_ptr<Entity>& entityToAttack, double elapsed_ms) {
	if (unitComp.state == UnitState::ATTACK) {
		// Already attacking something else, nothing to do, return.
		return;
	}

	if (aiComp.type != GamePieceClass::UNIT_OFFENSIVE) return;

	unitComp.state = UnitState::ATTACK;
	entityToAttack->takeAttack(*this, elapsed_ms);

	// Check to see if attack is done.
	// Set state to non-attacking state if attack is done (other entity is killed)
	if (entityToAttack->aiComp.currentHealth <= 0) {
		unitComp.state = UnitState::IDLE;
	}
}

void PivotingGunEntity::animate(float ms) {
	glm::vec3 dir;
	if (target) { // http://www.cplusplus.com/reference/memory/shared_ptr/operator%20bool/
		targetPosition = target->getPosition();
		dir = glm::normalize(targetPosition - getPosition());
		if (attackingCooldown >= 0)attackingCooldown -= ms;
		if (attackingCooldown < 0) {
			glm::vec3 start = getPosition();
			glm::vec3 end = target->getPosition();
			attackingCooldown = 1000.0f / unitComp.attackSpeed;
			float speed = unitComp.attackRange / attackingCooldown;
			float distance = glm::length(end - start);
			float lifespan = distance / speed;
			Global::weapons.push_back(
				std::make_shared<ProjectileWeapon>(weaponMesh, start + offset, end + glm::vec3(0, 0.5, 0), lifespan)
			);
		}
	}
	else {
		dir = glm::normalize(getPosition()+glm::vec3(1.0f,0.0f,0.0f));;
	}
	setRotationXZ(turretIndex, dir);
}

void PivotingGunEntity::attack(const std::shared_ptr<Entity>& entityToAttack, double elapsed_ms) {

	if (unitComp.state == UnitState::ATTACK) {
		// Already attacking something else, nothing to do, return.
		return;
	}

	if (aiComp.type != GamePieceClass::UNIT_OFFENSIVE) return;

	unitComp.state = UnitState::ATTACK;
	entityToAttack->takeAttack(*this, elapsed_ms);
	target = entityToAttack;
	// Check to see if attack is done.
	// Set state to non-attacking state if attack is done (other entity is killed)
	if (entityToAttack->aiComp.currentHealth <= 0) {
		unitComp.state = UnitState::IDLE;
	}
}

void BeamFiringGunEntity::animate(float ms)
{
	glm::vec3 dir;
	if (target) { // http://www.cplusplus.com/reference/memory/shared_ptr/operator%20bool/
		targetPosition = target->getPosition();
		dir = glm::normalize(targetPosition - getPosition());
		if (attackingCooldown >= 0)attackingCooldown -= ms;
		if (attackingCooldown < 0) {
			glm::vec3 start = getPosition();
			glm::vec3 end = target->getPosition();
			attackingCooldown = 1000.0f / unitComp.attackSpeed;
			Global::weapons.push_back(
				std::make_shared<BeamWeapon>(weaponMesh, start + offset, end + glm::vec3(0,0.5,0), attackingCooldown/1.5f)
			);
		}
	}
	else {
		dir = glm::normalize(getPosition() + glm::vec3(1.0f, 0.0f, 0.0f));;
	}
	setRotationXZ(turretIndex, dir);
}
