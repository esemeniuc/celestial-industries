// Header
#include "tile.hpp"
#include "objloader.hpp"
#include "global.hpp"

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>


// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"


GunTowerTile::GunTowerTile(): Tile(Model::MeshType::GUN_TURRET)
{
     randomDistribution = std::uniform_real_distribution<double>(0.0, 1.0);
}

void GunTowerTile::move(double ms)
{
    timeCounter += ms;
    if (exploding) {
        for (size_t i = 1; i < explosionDirections.size(); i++) {
			geometryRenderer.translate(i, explosionDirections[i] * (float)(ms*explosionVelocity / 1000.0f), false);
            /*rotate(i, explosionVelocity*ms/1000, explosionRotationalAxes[i], false);*/
        }
    }
    else {
        float period = 50;
        glm::vec3 gunDisplacement = { 0.0, 0.0, 0.1 };
        float amountToMove = sin(timeCounter / period);
		geometryRenderer.setModelMatrix(
            2,
            gunDisplacement*amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
        );
		geometryRenderer.setModelMatrix(
            3,
            -gunDisplacement * amountToMove + glm::vec3({ 0.0, 0.0, 0.1 })
        );

		geometryRenderer.rotate(1, ms / 1000, { 0,1,0 });
    }
}

void Tile::moveTo(UnitState unitState, int x, int z, bool queueMove)
{
	// Do nothing
}

void Tile::removeSelf()
{
	softDelete();
}

void GunTowerTile::explode(glm::vec3 dir)
{
    exploding = true;
    explosionVelocity = randomDistribution(randomEngine) * 10.0f - 5.0f;
	geometryRenderer.setModelMatricesFromComputed();
    for (size_t i = 0; i < geometryRenderer.parent->subObjects.size(); i++) {
        // Calculate a random direction to send the subobject in and give it a velocity.
        glm::vec3 randomOffset = glm::normalize(glm::vec3(randomDistribution(randomEngine), randomDistribution(randomEngine), randomDistribution(randomEngine)));
        glm::vec3 result = glm::normalize(dir + randomOffset);
        explosionRotationalAxes.push_back(
            glm::normalize(glm::vec3(randomDistribution(randomEngine), randomDistribution(randomEngine), randomDistribution(randomEngine)))
        );
        if (explosionVelocity < 0) {
            if (result.y > 0)
                result.y = -result.y;
        }
        explosionDirections.push_back(result);
    }
    timeCounter = 0.0f;
}

void GunTowerTile::animate(float ms)
{
	glm::vec3 dir;
	if (target) { // http://www.cplusplus.com/reference/memory/shared_ptr/operator%20bool/
		targetPosition = target->getPosition();
		dir = glm::normalize(targetPosition - getPosition());
	}
	else {
		dir = { 0,0,-1 };
	}
	setRotationXZ(1, dir);
}

void GunTowerTile::attack(const std::shared_ptr<Entity>& entityToAttack, double elapsed_ms)
{
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

Tile::Tile(Model::MeshType mesh) : Entity(mesh) {
	hasPhysics = false;
	type = mesh;
	rigidBody.removeSelf();
}
void Tile::update(double ms)
{
	// Because its called move on entities  -_-
	move(ms);
}

void RefineryTile::move(double ms)
{
	Global::playerResourcesFloat += ms / 1000.0f * resourceCollectionRate;
	Global::playerResources = (int)Global::playerResourcesFloat;
}

void RefineryTile::removeSelf()
{
	Global::playerResourcesPerSec -= resourceCollectionRate;
	Tile::removeSelf();
}

RefineryTile::RefineryTile(unsigned int numberOfGeysersReplaced): Tile(Model::MeshType::REFINERY)
{
	resourceCollectionRate = numberOfGeysersReplaced * 10;
	Global::playerResourcesPerSec += resourceCollectionRate;
	size = { 3, 0, 3 };
}

GeyserTile::GeyserTile(std::shared_ptr<Shader> particleShader, std::shared_ptr<Texture> particleTexture) : Tile(Model::MeshType::GEYSER)
{
	emitter = std::make_shared<Particles::ParticleEmitter>(
		position, // emitter position
		glm::vec3{ 0,1,0 }, // emitter direction
		0.8f,    // spread
		0.5f,    // particle width
		0.5f,    // particle height
		2.0f,    // lifespan
		5.0f,    // speed
		particleShader,
		particleTexture
		);
	Global::emitters.push_back(emitter);
}

void GeyserTile::setPosition(glm::vec3 position)
{
	emitter->setPosition(position);
	Tile::setPosition(position); // like calling super
}

void GeyserTile::removeSelf()
{
	emitter->isDeleted = true;
	Tile::removeSelf();
}
