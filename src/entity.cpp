#include "entity.hpp"
#include "global.hpp" //for pathfinding stuff

Entity::Entity() : geometryRenderer(Model::meshRenderers[Model::MeshType::BALL]) {}

Entity::Entity(Model::MeshType geometry) : geometryRenderer(Model::meshRenderers[geometry]) {}

//example of using the animate function when overriding Entity
void Entity::animate(float ms) {
	this->geometryRenderer.scale(glm::vec3(1.01, 1.01, 1.01)); //default implementation for demo purposes
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

void Entity::setTargetPath(const std::vector<Coord>& targetPath) {
	unitComp.targetPathStartTimestamp = 0;
	unitComp.targetPath = targetPath;
}

void Entity::moveTo(int x, int z) {
	setTargetPath(AI::aStar::a_star(aiCostMap, 1, (int) rigidBody.getPosition().x, (int) rigidBody.getPosition().z, x,
									z).second); //might need fixing with respect to int start positions

}

//returns a pathIndex and a 0.00 - 0.99 value to interpolate between steps in a path
std::pair<int, double> Entity::getInterpolationPercentage() {
	double intermediateVal = (unitComp.targetPathStartTimestamp / 1000) * unitComp.movementSpeed;
	int pathIndex = (int) intermediateVal;
	double interpolationPercent = clamp<double>(0, intermediateVal - pathIndex, 1);
	return {pathIndex, interpolationPercent};
}

void Entity::move(double elapsed_time) {
	if (unitComp.targetPath.empty()) {
		return;
	}

	unitComp.targetPathStartTimestamp += elapsed_time;
	std::pair<int, double> index = getInterpolationPercentage(); //first is index into path, second is interp amount (0 to 1)
	if (index.first < (int) unitComp.targetPath.size() - 1) {
		Coord curr = unitComp.targetPath[index.first];
		Coord next = unitComp.targetPath[index.first + 1];

		double dRow = next.rowCoord - curr.rowCoord;
		double dCol = next.colCoord - curr.colCoord;

//			double transRow = (dRow / (1000 / elapsed_time)) * movementSpeed;
//			double transCol = (dCol / (1000 / elapsed_time)) * movementSpeed;
//			translate({transCol, 0, transRow});

		double destCol = curr.colCoord + (dCol * index.second);
		double destRow = curr.rowCoord + (dRow * index.second);

		glm::vec3 newPos = {destCol, 0, destRow};
		setPositionFast(0, newPos); //for rendering
		rigidBody.setPosition(newPos); //for phys
	} else { //move to the last coord in the path
		glm::vec3 newPos = {unitComp.targetPath.back().colCoord, 0, unitComp.targetPath.back().rowCoord};
		setPositionFast(0, newPos);
		rigidBody.setPosition(newPos);
	}
}

glm::vec3 Entity::getPosition() const {
	return rigidBody.getPosition();
}

bool Entity::canSee(std::shared_ptr<Entity> entity) {
	return glm::length(glm::vec2(entity->getPosition() - this->getPosition())) <= aiComp.visionRange;
}

bool Entity::inAttackRange(std::shared_ptr<Entity> entity) {
	return glm::length(glm::vec2(entity->getPosition() - this->getPosition())) <= unitComp.attackRange;
}

bool Entity::operator==(const Entity& rhs) const {
	return geometryRenderer == rhs.geometryRenderer &&
		   aiComp == rhs.aiComp &&
		   unitComp == rhs.unitComp &&
		   rigidBody == rhs.rigidBody;
}

void Entity::takeAttack(Entity& attackingEntity, double elapsed_ms) {
    // reduce health
    int damagePerSecond = attackingEntity.unitComp.attackDamage * attackingEntity.unitComp.attackSpeed;
    float damageToDoThisFrame = damagePerSecond * (elapsed_ms / 1000);

    aiComp.currentHealth -= damageToDoThisFrame;
}

void Entity::attack(std::shared_ptr<Entity> entityToAttack, double elapsed_ms) {
    if (unitComp.state == UnitState::ATTACK) {
		logger << 'already attacking this frame';
        // Already attacking something else, nothing to do, return.
        return;
    }

	logger << " 1 attack done" << '\n';

	if (aiComp.type != GamePieceClass::UNIT_OFFENSIVE) return;

    unitComp.state = UnitState::ATTACK;
    entityToAttack->takeAttack(*this, elapsed_ms);

    // Check to see if attack is done.
    // Set state to non-attacking state if attack is done (other entity is killed)
    if (entityToAttack->aiComp.currentHealth <= 0) {
        unitComp.state = UnitState::IDLE;
    }
}
