#include "weapons.hpp"

Weapon::Weapon(Model::MeshType type, glm::vec3 _start, glm::vec3 _end)
{
	geometryRenderer = Renderable(Model::meshRenderers[type]);
	timeSinceStart = 0;
	start = _start;
	end = _end;
	isDeleted = false;
	glm::vec3 dir = end - start;
	glm::mat4 rotationMatrix = glm::orientation(dir, { 0,0, -1 });
	//rotationMatrix = glm::translate(rotationMatrix, start);
	//geometryRenderer.setModelMatrix(0, rotationMatrix);
	geometryRenderer.setModelMatrix(0, start);
}

void Weapon::update(double ms)
{
	timeSinceStart += ms;
}

void Weapon::softDelete()
{
	geometryRenderer.removeSelf();
	isDeleted = true;
}

BeamWeapon::BeamWeapon(Model::MeshType type, glm::vec3 start, glm::vec3 end, double _lifespan) : Weapon(type, start, end)
{
	lifespan = _lifespan;
}

void BeamWeapon::update(double ms)
{
	timeSinceStart += ms;
	if (timeSinceStart > ms) {
		softDelete();
	}
	else {

	}
}
