//
// Created by eric on 2/26/18.
//

#pragma once

#include <vector>
#include "common.hpp"

enum class UnitState {
	IDLE, RECHARGING, LOW_ENERGY, ATTACK, ATTACK_MOVE, MOVE, RETREAT, ACTION
};


//assumes we have position from entity class
class UnitComp {
public:

	//immutable values
	const int initialEnergyLevel;
	const int attackDamage;
	const int attackRange; //range in tiles
	const int attackSpeed; //attacks per second
	const int movementSpeed; //units per second, maybe make non const later for energy effects

	//mutable values
	int currentEnergyLevel;
	UnitState state;
	std::vector<Coord> targetPath;
	double targetPathStartTimestamp; //needed to get delta time


	UnitComp() : initialEnergyLevel(50),
				 attackDamage(6),
				 attackRange(6),
				 attackSpeed(1),
				 movementSpeed(15),
				 currentEnergyLevel(50),
				 state(UnitState::IDLE) {
	}


	UnitComp(int _initialHealth,
			 int _initialEnergyLevel,
			 int _attackDamage,
			 int _attackRange,
			 int _attackSpeed,
			 int _movementSpeed,
			 int _visionRange,
			 int _unitValue,
			 UnitState _state) : initialEnergyLevel(_initialEnergyLevel),
								 attackDamage(_attackDamage),
								 attackRange(_attackRange),
								 attackSpeed(_attackSpeed),
								 movementSpeed(_movementSpeed),
								 currentEnergyLevel(_initialEnergyLevel),
								 state(_state) {
	}

	void isLowHealth()
	{
		
	}

	void update() {

//		if (Health == 100 && triggerL == false && triggerR == false && triggerM
//																	   == false) {
//			statePassive = true;
//			stateAggressive = false;
//			stateDefensive = false;
//		}

	}
};

//https://softwareengineering.stackexchange.com/questions/253704/when-is-type-testing-ok
class RangedUnit : public UnitComp {

public:

	RangedUnit() : UnitComp(100, 50, 10, 6, 1, 1, 6, 50,
							UnitState::IDLE) {
		logger(LogLevel::DEBUG) << "ranged unit built" << Logger::endl;
	}

};
