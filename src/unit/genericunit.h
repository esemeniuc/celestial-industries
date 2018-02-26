//
// Created by eric on 2/26/18.
//

#pragma once

#include "entity.hpp"
#include "common.hpp"

enum class UnitState {
	IDLE, RECHARGING, LOW_ENERGY, ATTACK, RETREAT, ACTION
};

enum class UnitOwner {
	PLAYER, AI
};

//assumes we have position from entity class
class GenericUnit : public Entity {
private:


protected:
	//immutable values
	int initialHealth = 100;
	int initialEnergyLevel = 50;
	int attackDamage = 10; //
	int attackRange = 6; //range in tiles
	int attackSpeed = 1; //attacks per second
	int movementSpeed = 1; //tiles per second, maybe make non const later for energy effects
	int unitValue = 50;
	UnitOwner owner = UnitOwner::PLAYER;
	//mutable values
	int currentHealth;
	int currentEnergyLevel;
	UnitState state;

public:

	GenericUnit(int _initialHealth, int _initialEnergyLevel, int _attackDamage, int _attackRange, int _attackSpeed,
				int _movementSpeed, int _unitValue, UnitOwner _owner, int _currentHealth, int _currentEnergyLevel,
				UnitState _state) : initialHealth(_initialHealth), initialEnergyLevel(_initialEnergyLevel),
								   attackDamage(_attackDamage), attackRange(_attackRange), attackSpeed(_attackSpeed),
								   movementSpeed(_movementSpeed), unitValue(_unitValue), owner(_owner),
								   currentHealth(_currentHealth), currentEnergyLevel(_currentEnergyLevel), state(_state) {}

	void update(float ms) override {

	}

};

//https://softwareengineering.stackexchange.com/questions/253704/when-is-type-testing-ok
class RangedUnit : public GenericUnit {
	//stuff
public:
	explicit RangedUnit() : GenericUnit(100, 50, 10, 6, 1, 1, 50,
										UnitOwner::PLAYER, initialHealth,
										initialEnergyLevel, UnitState::IDLE) {
		logger(LogLevel::DEBUG) << "unit built" << Logger::endl;
	}

};