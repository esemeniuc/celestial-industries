#pragma once

#include <memory>
#include "entity.hpp"
#include "global.hpp"

namespace Unit {

	std::shared_ptr<Entity> spawn(Model::MeshType type, glm::vec3 location, GamePieceOwner owner);

	std::shared_ptr<Entity> entityFromMeshType(Model::MeshType type);
	std::shared_ptr<Entity> spawn(UnitType unitName, glm::vec3 spawnLocation, GamePieceOwner owner);
}
