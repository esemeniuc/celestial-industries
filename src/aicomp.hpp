#pragma once

#include "rigidBody.hpp"
#include <memory>

//make sure to update entityinfo.cpp if you add new enums here
enum class GamePieceOwner {
	NONE,
	PLAYER,
	AI,
};

enum class GamePieceClass {
	NONE,
	BUILDING_NON_ATTACKING,
	UNIT_NON_ATTACKING,
	BUILDING_DEFENSIVE_PASSIVE,
	BUILDING_DEFENSIVE_ACTIVE,
	UNIT_DEFENSIVE_ACTIVE,
	UNIT_OFFENSIVE,
};


class AiComp {

public:
	//members
	int totalHealth = 100;
	int visionRange = 5;
	GamePieceOwner owner = GamePieceOwner::NONE;
	GamePieceClass type = GamePieceClass::NONE;
	float currentHealth = totalHealth;
	int value = 0; //used for AI to prioritize targets

	//constructors
	AiComp() {}

	AiComp(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceClass type, int value)
			: totalHealth(initialHealth),
			  visionRange(visionRange),
			  owner(owner),
			  type(type),
			  currentHealth((float) initialHealth),
			  value(value) {}

	AiComp(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceClass type,
		   int currentHealth, int value) : totalHealth(initialHealth),
										   visionRange(visionRange),
										   owner(owner), type(type),
										   currentHealth((float) currentHealth),
										   value(value) {}


	bool operator==(const AiComp& rhs) const {
		return totalHealth == rhs.totalHealth &&
			   visionRange == rhs.visionRange &&
			   owner == rhs.owner &&
			   type == rhs.type &&
			   currentHealth == rhs.currentHealth &&
			   value == rhs.value;
	}
};


