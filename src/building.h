#include <entity.hpp>


enum class DefensiveBuildingState {
	IDLE, PIVOT, ATTACK
};

class Building : public Entity {
protected:
	const int initialHealth;
	//for ai to calculate what to attack
	int currentHealth;

public:
	Building() : initialHealth(100), buildingValue(100) {

	}

	void animate(float ms) override {

	}

	const int buildingValue;
};


class DefensiveBuilding : Building {
public:
	DefensiveBuilding() {}

	void animate(float ms) override {

	}

protected:

	int attackPower;
	int attackRange;
	int attackSpeed;
	DefensiveBuildingState state;
};
