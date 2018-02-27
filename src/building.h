//
// Created by eric on 2/26/18.
//


#include <entity.hpp>

class Building : public Entity {
protected:
	const int initialHealth;
    //for ai to calculate what to attack
	int currentHealth;
public:
	Building():initialHealth(100), buildingValue(100) {

	}
    const int buildingValue;
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