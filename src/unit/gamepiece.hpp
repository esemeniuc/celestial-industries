#pragma once

#include <memory>
#include <utility>
#include "rigidBody.hpp"

enum class GamePieceOwner {
	NONE, PLAYER, AI
};

enum class GamePieceType {
	NONE, NON_ATTACKING, DEFENSIVE_PASSIVE, DEFENSIVE_ACTIVE, OFFENSIVE
};


class AiInfo {

public:
	//members
	const int initialHealth;
	const int visionRange;
	GamePieceOwner owner = GamePieceOwner::NONE;
	GamePieceType type = GamePieceType::NONE;
	int currentHealth;
	int value; //used for AI to prioritize targets



	//constructors
	AiInfo() : initialHealth(100),
				  visionRange(5) {}

	AiInfo(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type, int value)
			: initialHealth(initialHealth),
			  visionRange(visionRange),
			  owner(owner),
			  type(type),
			  currentHealth(initialHealth),
			  value(value) {}

	AiInfo(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type,
			  int currentHealth, int value) : initialHealth(initialHealth),
											  visionRange(visionRange),
											  owner(owner), type(type),
											  currentHealth(currentHealth),
											  value(value) {}


};


