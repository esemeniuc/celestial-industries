//
// Created by eric on 2/26/18.
//


#include <entity.hpp>

class Building : Entity {
protected:
	int initialHealth;
	const int buildingValue; //for ai to calculate what to attack

	int currentHealth;
};


enum class DefenceBuildingState {
	IDLE, PIVOT, ATTACK
};

class DefenceBuilding : Building {
protected:
	int attackPower;
	int attackRange;
	int attackSpeed;
};