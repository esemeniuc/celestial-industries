//
// Created by eric on 4/11/18.
//
#include "model.hpp"
#include "entityinfo.hpp"
std::array<const char*, Model::MeshType::MESHTYPES_COUNT> init() {
	std::array<const char*, Model::MeshType::MESHTYPES_COUNT> tempNameLookupTable{};
	tempNameLookupTable[Model::MeshType::REFINERY] = "Refinery";
	tempNameLookupTable[Model::MeshType::MINING_TOWER] = "Mining Tower";
	tempNameLookupTable[Model::MeshType::PHOTON_TOWER] = "Photon Tower";
	tempNameLookupTable[Model::MeshType::SUPPLY_DEPOT] = "Supply Depot";
	tempNameLookupTable[Model::MeshType::BALL] = "Ball";
	tempNameLookupTable[Model::MeshType::GUN_TURRET] = "Gun Turret";
	tempNameLookupTable[Model::MeshType::GEYSER] = "Resource Geyser";
	tempNameLookupTable[Model::MeshType::ENEMY_SPIKE_UNIT] = "Spiked Melee";
	tempNameLookupTable[Model::MeshType::ENEMY_RANGED_LINE_UNIT] = "Line Attack";
	tempNameLookupTable[Model::MeshType::ENEMY_RANGED_RADIUS_UNIT] = "Splash Attack";
	tempNameLookupTable[Model::MeshType::FRIENDLY_FIRE_UNIT] = "Flamer";
	tempNameLookupTable[Model::MeshType::FRIENDLY_RANGED_UNIT] = "Ranged";
	return tempNameLookupTable;
}


namespace EntityInfo {
		std::array<const char*, Model::MeshType::MESHTYPES_COUNT> nameLookupTable = init();


}
