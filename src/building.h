//
// Created by eric on 2/26/18.
//


#include <entity.hpp>

class Building : Entity {
protected:
	const int initialHealth;
	const int buildingValue; //for ai to calculate what to attack

	int currentHealth;
};


enum class DefensiveBuildingState {
	IDLE, PIVOT, ATTACK
};

class DefensiveBuilding : Building {
protected:

	int attackPower;
	int attackRange;
	int attackSpeed;
	DefensiveBuildingState state;
};