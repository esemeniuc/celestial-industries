//
// Created by eric on 2/26/18.
//

#pragma once

#include <vector>
#include "common.hpp"
#include "entity.hpp"
#include "pathfinder.hpp"
#include "gamepiece.hpp"

enum class UnitState {
	IDLE, RECHARGING, LOW_ENERGY, ATTACK, RETREAT, ACTION
};


//assumes we have position from entity class
class GenericUnit : public GamePiece {
private:


protected:
	//immutable values
	const int initialEnergyLevel;
	const int attackDamage;
	const int attackRange; //range in tiles
	const int attackSpeed; //attacks per second
	const int movementSpeed; //tiles per second, maybe make non const later for energy effects

	//mutable values
	int currentEnergyLevel;
	UnitState state;
	std::vector<Coord> targetPath;
	float targetPathStartTimestamp; //needed to get delta time

public:

	GenericUnit() : initialEnergyLevel(50),
					attackDamage(6),
					attackRange(6),
					attackSpeed(1),
					movementSpeed(1),
					currentEnergyLevel(50),
					state(UnitState::IDLE),
					GamePiece(100, 6, GamePieceOwner::NONE, GamePieceType::NONE, 50) {

		entity = std::make_shared<Entity>(Model::MeshType::BALL);
	}

	GenericUnit(Model::MeshType _meshType) : initialEnergyLevel(50),
											 attackDamage(6),
											 attackRange(6),
											 attackSpeed(1),
											 movementSpeed(1),
											 currentEnergyLevel(50),
											 state(UnitState::IDLE),
											 GamePiece(100, 6, GamePieceOwner::NONE, GamePieceType::NONE, 50) {

		entity = std::make_shared<Entity>(_meshType);
	}

	GenericUnit(int _initialHealth,
				int _initialEnergyLevel,
				int _attackDamage,
				int _attackRange,
				int _attackSpeed,
				int _movementSpeed,
				int _visionRange,
				int _unitValue,
				GamePieceOwner _owner,
				GamePieceType _type,
				UnitState _state,
				Model::MeshType _meshType) : initialEnergyLevel(_initialEnergyLevel),
											 attackDamage(_attackDamage),
											 attackRange(_attackRange),
											 attackSpeed(_attackSpeed),
											 movementSpeed(_movementSpeed),
											 currentEnergyLevel(_initialEnergyLevel),
											 state(_state),
											 GamePiece(_initialHealth, _visionRange, _owner, type, _unitValue) {
	}

	void move(float elapsed_time) {
		elapsed_time / movementSpeed;
	}


	bool inVisionRange(const GamePiece& _gamePiece) {
		return glm::length(glm::vec2(_gamePiece.rigidBody.getPosition() - rigidBody.getPosition())) <= visionRange;
	}

	bool inAttackRange(const GamePiece& _gamePiece) {
		return glm::length(glm::vec2(_gamePiece.rigidBody.getPosition() - rigidBody.getPosition())) <= attackRange;
	}

	void setTargetPath(const std::vector<Coord>& targetPath) {
		GenericUnit::targetPath = targetPath;
	}
};

//https://softwareengineering.stackexchange.com/questions/253704/when-is-type-testing-ok
class RangedUnit : public GenericUnit {

public:


	RangedUnit(Model::MeshType _meshType) : GenericUnit(100, 50, 10, 6, 1, 1, 6, 50,
														GamePieceOwner::PLAYER,
														GamePieceType::OFFENSIVE,
														UnitState::IDLE,
														_meshType) {
		logger(LogLevel::DEBUG) << "ranged unit built" << Logger::endl;
	}

};
