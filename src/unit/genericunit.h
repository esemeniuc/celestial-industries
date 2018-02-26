//
// Created by eric on 2/26/18.
//

#pragma once
#include "entity.hpp"

enum class UnitState
{
	IDLE, RECHARGING, LOW_ENERGY, ATTACK, RETREAT, ACTION
};

//state transitions


class GenericUnit : public Entity
{
	int health;
	int energyLevel;
	int attackPower;
	int attackRange;

	//assumes we have position from entity class

	// I wonder if we should include some info about the unit obj
	//objective?

public:
	GenericUnit(int health, int energyLevel, int attackPower, int attackRange) : health(health),
																				 energyLevel(energyLevel),
																				 attackPower(attackPower),
																				 attackRange(attackRange) {}

	void update(float ms) override {

	}

};
