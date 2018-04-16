#include "weapons.hpp"
#include "global.hpp"

Weapon::Weapon(Model::MeshType type, glm::vec3 _start, glm::vec3 _end, double _lifespan)
{
	geometryRenderer = Renderable(Model::meshRenderers[type]);
	timeSinceStart = 0;
	start = _start;
	end = _end;
	lifespan = _lifespan;
	isDeleted = false;
}

void Weapon::update(double ms)
{
	if (timeSinceStart > lifespan)
		return;
	timeSinceStart += ms;
	// Only delete once
	if (timeSinceStart > lifespan)
		softDelete();
	glm::vec3 dir = end - start;
	float t = timeSinceStart / lifespan;
	glm::vec3 pos = start * (1 - t) + end * t;
	glm::mat4 rotationMatrix = glm::orientation(glm::normalize(dir), { 0, 0, -1 });
	glm::mat4 translationMatrix = glm::translate(pos);
	glm::mat4 result = translationMatrix * rotationMatrix;
	geometryRenderer.setModelMatrix(0, result);
}

void Weapon::softDelete()
{
	geometryRenderer.removeSelf();
	isDeleted = true;
}

virtual Weapon::~Weapon() = default;

BeamWeapon::BeamWeapon(Model::MeshType type, glm::vec3 start, glm::vec3 end, double lifespan) : Weapon(type, start, end, lifespan) {}

void BeamWeapon::update(double ms)
{
	if (timeSinceStart > lifespan)
		return;
	timeSinceStart += ms;
	// Only delete once
	if (timeSinceStart > lifespan)
		softDelete();
	glm::vec3 dir = end - start;
	float t = timeSinceStart / lifespan;
	glm::mat4 rotationMatrix = glm::orientation(glm::normalize(dir), { 0, 0, -1 });
	glm::mat4 translationMatrix = glm::translate(start+dir/2.0f);
	glm::mat4 scalingMatrix = glm::scale(glm::vec3(1,1,glm::length(dir)));
	glm::mat4 result = translationMatrix * rotationMatrix * scalingMatrix;
	geometryRenderer.setModelMatrix(0, result);
}

ProjectileWeapon::ProjectileWeapon(Model::MeshType type, glm::vec3 start, glm::vec3 end, double lifespan) : Weapon(type, start, end, lifespan) {}
