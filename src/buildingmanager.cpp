#include "buildingmanager.hpp"
#include "global.hpp"
#include "model.hpp"

namespace BuildingManager {

	void init(size_t levelHeight, size_t levelWidth) {
		Global::buildingMap.reserve(levelHeight * levelWidth);
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
		Global::buildingMap.erase(std::remove_if(Global::buildingMap.begin(), Global::buildingMap.end(), isDead), Global::buildingMap.end());
		// std::cout << "buildings after: " << playerUnits.size() << '\n';
	}

	void update(double elapsed_ms) {
		removeDead();
		for (auto& building : Global::buildingMap) {
			if (building->aiComp.currentHealth <= 0) {
				building->softDelete();
			}
		}
	}

	void selectBuilding(const glm::vec3 & targetLocation)
	{
		//Global::selectedBuildingType = Model::MeshType::NONE;

		if (Global::levelArray[targetLocation.x][targetLocation.y] == Model::MeshType::FACTORY) {
			//Global::selectedBuildingType = Model::MeshType::FACTORY;
			logger(LogLevel::INFO) << "Factory selected!\n";
		};
	}

}
