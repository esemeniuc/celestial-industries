//
// Created by eric on 2/26/18.
//


#include <entity.hpp>

class Building : Entity {
protected:
	int health;

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