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


class GamePiece {

public:
	//members
	const int initialHealth;
	const int visionRange;
	GamePieceOwner owner = GamePieceOwner::NONE;
	GamePieceType type = GamePieceType::NONE;
	int currentHealth;
	int value; //used for AI to prioritize targets



	//constructors
	GamePiece() : initialHealth(100),
				  visionRange(5) {}

	GamePiece(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type, int value)
			: initialHealth(initialHealth),
			  visionRange(visionRange),
			  owner(owner),
			  type(type),
			  currentHealth(initialHealth),
			  value(value) {}

	GamePiece(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type,
			  int currentHealth, int value) : initialHealth(initialHealth),
											  visionRange(visionRange),
											  owner(owner), type(type),
											  currentHealth(currentHealth),
											  value(value) {}


};


