//
// Created by eric on 2/26/18.
//

#pragma once

#include <queue>
#include "common.hpp"
#include "entity.hpp"

enum class UnitState {
	IDLE, RECHARGING, LOW_ENERGY, ATTACK, RETREAT, ACTION
};


//assumes we have position from entity class
class GenericUnit : public Entity {
private:


protected:
	//immutable values
	const int initialHealth;
	const int initialEnergyLevel;
	const int attackDamage;
	const int attackRange; //range in tiles
	const int attackSpeed; //attacks per second
	const int movementSpeed; //tiles per second, maybe make non const later for energy effects
	const int visionRange;
	const int unitValue;
	//mutable values
	int currentHealth;
	int currentEnergyLevel;
	std::queue<Coord> moveTargets;
	UnitState state;

public:

	GenericUnit(int _initialHealth,
				int _initialEnergyLevel,
				int _attackDamage,
				int _attackRange,
				int _attackSpeed,
				int _movementSpeed,
				int _visionRange,
				int _unitValue,
				EntityOwner _owner,
				int _currentHealth,
				int _currentEnergyLevel,
				UnitState _state,
				std::shared_ptr<Renderer> _parent) : initialHealth(_initialHealth),
													 initialEnergyLevel(_initialEnergyLevel),
													 attackDamage(_attackDamage),
													 attackRange(_attackRange),
													 attackSpeed(_attackSpeed),
													 movementSpeed(_movementSpeed),
													 visionRange(_visionRange),
													 unitValue(_unitValue),
													 currentHealth(_currentHealth),
													 currentEnergyLevel(_currentEnergyLevel),
													 state(_state), Entity(_parent) {
		owner = _owner;
	}

	void update(float ms) override {

	}

};

//https://softwareengineering.stackexchange.com/questions/253704/when-is-type-testing-ok
class RangedUnit : public GenericUnit {
	//stuff
public:
	explicit RangedUnit(std::shared_ptr<Renderer> _parent) : GenericUnit(100, 50, 10, 6, 1, 1, 6, 50,
																		 EntityOwner::PLAYER, initialHealth,
																		 initialEnergyLevel, UnitState::IDLE, _parent) {
		logger(LogLevel::DEBUG) << "unit built" << Logger::endl;
	}

};
