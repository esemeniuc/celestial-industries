#pragma once

#include <memory>
#include "entity.hpp"
#include "rigidBody.hpp"

enum class GamePieceOwner {
	NONE, PLAYER, AI
};

enum class GamePieceType {
	NONE, NON_ATTACKING, DEFENSIVE_PASSIVE, DEFENSIVE_ACTIVE, OFFENSIVE
};


class GamePiece {

protected:
	//members
	const int initialHealth;
	const int visionRange;
	GamePieceOwner owner = GamePieceOwner::NONE;
	GamePieceType type = GamePieceType::NONE;
	int currentHealth;
	int value; //used for AI to prioritize targets
	std::shared_ptr<Entity> entity; //for being drawn

public:
	RigidBody rigidBody;


	GamePiece(const int initialHealth, const int visionRange) : initialHealth(initialHealth),
																visionRange(visionRange) {}

	GamePiece(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type, int value)
			: initialHealth(initialHealth),
			  visionRange(visionRange),
			  owner(owner),
			  type(type),
			  currentHealth(initialHealth),
			  value(value) {}

	GamePiece(const int initialHealth, const int visionRange, GamePieceOwner owner, GamePieceType type,
			  int currentHealth, int value, const std::shared_ptr<Entity>& entity) : initialHealth(initialHealth),
																					 visionRange(visionRange),
																					 owner(owner), type(type),
																					 currentHealth(currentHealth),
																					 value(value), entity(entity) {}
};


