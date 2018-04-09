//
// Created by eric on 2/26/18.
//

#pragma once

#include <vector>
#include "coord.hpp"

enum class UnitState {
	NONE, IDLE, RECHARGING, LOW_ENERGY, ATTACK, ATTACK_MOVE, MOVE, SCOUT, RETREAT, ACTION
};


//assumes we have position from entity class
class UnitComp {
public:
	int initialEnergyLevel;
	int attackDamage;
	int attackRange; //range in tiles
	int attackSpeed; //attacks per second
	int movementSpeed; //units per second, maybe make non const later for energy effects

	//mutable values
	int currentEnergyLevel;
	UnitState state;
	std::vector<Coord> targetPath;
	Coord targetDest;
	double targetPathStartTimestamp = 0; //needed to get delta time

	glm::vec3 velocity;
	glm::vec3 nextPos;


	UnitComp() : initialEnergyLevel(50),
				 attackDamage(1),
				 attackRange(5),
				 attackSpeed(1),
				 movementSpeed(10),
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

	void update() {

//		if (Health == 100 && triggerL == false && triggerR == false && triggerM
//																	   == false) {
//			statePassive = true;
//			stateAggressive = false;
//			stateDefensive = false;
//		}

	}

	bool operator==(const UnitComp& rhs) const {
		return initialEnergyLevel == rhs.initialEnergyLevel &&
			   attackDamage == rhs.attackDamage &&
			   attackRange == rhs.attackRange &&
			   attackSpeed == rhs.attackSpeed &&
			   movementSpeed == rhs.movementSpeed &&
			   currentEnergyLevel == rhs.currentEnergyLevel &&
			   state == rhs.state &&
			   targetPath == rhs.targetPath &&
			   targetPathStartTimestamp == rhs.targetPathStartTimestamp;
	}
};