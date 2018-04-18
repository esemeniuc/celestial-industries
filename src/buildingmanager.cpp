#include "buildingmanager.hpp"
#include "global.hpp"
#include "model.hpp"
#include "world.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		Global::buildingList.reserve(levelHeight * levelWidth);
	}

	bool isDead(std::shared_ptr<Entity>& unit)
	{
		if (unit->aiComp.currentHealth <= 0)
		{
			unit->softDelete();
		}
		return unit->aiComp.currentHealth <= 0;
	}

	void removeDead() {
		// std::cout << "buildings before: " << building.size() << '\n';
		Global::buildingList.erase(std::remove_if(Global::buildingList.begin(), Global::buildingList.end(), isDead), Global::buildingList.end());
		// std::cout << "buildings after: " << playerUnits.size() << '\n';
	}

	void update(double elapsed_ms) {
		removeDead();
		for (auto& building : Global::buildingList) {
			if (building->aiComp.currentHealth <= 0) {
				building->softDelete();
			}
		}
	}

	void selectBuilding(const glm::vec3 & targetLocation)
	{
		std::shared_ptr<Tile> selectedTile = World::level.getTileAt(targetLocation);
		if (selectedTile->meshType == Model::MeshType::FACTORY) {
			Global::selectedBuilding = selectedTile;
		} else {
			Global::selectedBuilding = nullptr;
		};
	}
}
