//
// Created by eric on 2/26/18.
//

#pragma once

#include "unitcomp.hpp"

//have groups knowing about their units, so the ai just manage the groups
class UnitGroup //can be owned by player or ai
{
	std::vector<GenericUnit> units;
};

//ai controlled attack group
class AttackGroup : public UnitGroup {
	GenericUnit attackGoal;

};