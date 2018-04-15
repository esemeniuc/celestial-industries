//
// Created by eric on 4/11/18.
//
#include "model.hpp"
#include "entityinfo.hpp"

std::array<const char*, Model::MeshType::MESHTYPES_COUNT> initNameLookupTable() {
	std::array<const char*, Model::MeshType::MESHTYPES_COUNT> tempNameLookupTable{};
	//buildings
	tempNameLookupTable[Model::MeshType::REFINERY] = "Refinery";
	tempNameLookupTable[Model::MeshType::MINING_TOWER] = "Mining Tower";
	tempNameLookupTable[Model::MeshType::PHOTON_TOWER] = "Photon Tower";
	tempNameLookupTable[Model::MeshType::SUPPLY_DEPOT] = "Supply Depot";
	tempNameLookupTable[Model::MeshType::GUN_TURRET] = "Gun Turret";
	tempNameLookupTable[Model::MeshType::FACTORY] = "Factory";
	tempNameLookupTable[Model::MeshType::COMMAND_CENTER] = "Command center";


	//geyser
	tempNameLookupTable[Model::MeshType::GEYSER] = "Resource Geyser";

	//units
	tempNameLookupTable[Model::MeshType::BALL] = "Ball";
	tempNameLookupTable[Model::MeshType::ENEMY_SPIKE_UNIT] = "Spiked Melee";
	tempNameLookupTable[Model::MeshType::ENEMY_RANGED_LINE_UNIT] = "Line Attack";
	tempNameLookupTable[Model::MeshType::ENEMY_RANGED_RADIUS_UNIT] = "Splash Attack";
	tempNameLookupTable[Model::MeshType::FRIENDLY_FIRE_UNIT] = "Flamer";
	tempNameLookupTable[Model::MeshType::FRIENDLY_RANGED_UNIT] = "Ranged";
	return tempNameLookupTable;
}

std::unordered_map<GamePieceClass, const char*> initGamePieceClassLookupTable() {
	std::unordered_map<GamePieceClass, const char*> tempLookupTable;
	tempLookupTable[GamePieceClass::NONE] = "None";
	tempLookupTable[GamePieceClass::BUILDING_NON_ATTACKING] = "Non-attacking Building";
	tempLookupTable[GamePieceClass::UNIT_NON_ATTACKING] = "Non-attacking Unit";
	tempLookupTable[GamePieceClass::BUILDING_DEFENSIVE_PASSIVE] = "Defensive Passive Building";
	tempLookupTable[GamePieceClass::BUILDING_DEFENSIVE_ACTIVE] = "Defensive Active Building";
	tempLookupTable[GamePieceClass::UNIT_DEFENSIVE_ACTIVE] = "Defensive Unit";
	tempLookupTable[GamePieceClass::UNIT_OFFENSIVE] = "Offensive Unit";

	return tempLookupTable;
}

std::unordered_map<GamePieceOwner, const char*> initGamePieceOwnerLookupTable() {
	std::unordered_map<GamePieceOwner, const char*> tempLookupTable;
	tempLookupTable[GamePieceOwner::NONE] = "None";
	tempLookupTable[GamePieceOwner::AI] = "Hostile";
	tempLookupTable[GamePieceOwner::PLAYER] = "Friendly";
	return tempLookupTable;
}


namespace EntityInfo {
	std::array<const char*, Model::MeshType::MESHTYPES_COUNT> nameLookupTable = initNameLookupTable();
	std::unordered_map<GamePieceClass, const char*> gamePieceClassLookupTable = initGamePieceClassLookupTable();
	std::unordered_map<GamePieceOwner, const char*> gamePieceOwnerLookupTable = initGamePieceOwnerLookupTable();
}
