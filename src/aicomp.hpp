#pragma once

#include "rigidBody.hpp"

enum class GamePieceOwner {
	NONE, PLAYER, AI
};

enum class GamePieceType {
	NONE, BUILDING_NON_ATTACKING, UNIT_NON_ATTACKING, BUILDING_DEFENSIVE_PASSIVE, BUILDING_DEFENSIVE_ACTIVE, UNIT_DEFENSIVE_ACTIVE, UNIT_OFFENSIVE
};


class AiComp {

public:
	//members
	const int initialHealth;
	const int visionRange;
	GamePieceOwner owner = GamePieceOwner::NONE;
	GamePieceType type = GamePieceType::NONE;
	int currentHealth;
	int value; //used for AI to prioritize targets



	//constructors
	AiComp() : initialHealth(100),
				  visionRange(5) {}

	AiComp(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type, int value)
			: initialHealth(initialHealth),
			  visionRange(visionRange),
			  owner(owner),
			  type(type),
			  currentHealth(initialHealth),
			  value(value) {}

	AiComp(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type,
			  int currentHealth, int value) : initialHealth(initialHealth),
											  visionRange(visionRange),
											  owner(owner), type(type),
											  currentHealth(currentHealth),
											  value(value) {}


};


