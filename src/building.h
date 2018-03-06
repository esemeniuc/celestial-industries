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
	Building(std::shared_ptr<Renderer> _parent):initialHealth(100), buildingValue(100), Entity(_parent) {

	}

	void update(float ms) override {

	}

    const int buildingValue;
};


enum class DefensiveBuildingState {
	IDLE, PIVOT, ATTACK
};

class DefensiveBuilding : Building {
public:
	DefensiveBuilding(std::shared_ptr<Renderer> _parent): Building(_parent) {}
	void update(float ms) override {

	}
protected:

	int attackPower;
	int attackRange;
	int attackSpeed;
	DefensiveBuildingState state;
};
