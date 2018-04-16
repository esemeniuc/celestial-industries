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
	std::vector<glm::vec3> targetPath;
	double targetPathStartTimestamp = 0; //needed to get delta time

	UnitComp() : initialEnergyLevel(50),
				 attackDamage(1),
				 attackRange(5),
				 attackSpeed(1),
				 movementSpeed(10),
				 currentEnergyLevel(50),
				 state(UnitState::IDLE) {
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