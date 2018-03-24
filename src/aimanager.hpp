//
// Created by eric on 2/25/18.
//

#pragma once

#include <set>
#include "global.hpp"
#include "unitcomp.hpp"
#include "building.hpp"

//assume AI has goal to destroy main control building of player
/*we do the tree thing like alla suggested
with look ahead to a depth level of 8 for example, 2^8 = 256
there we look for possible group actions
scan through the list of known player buildings with a attack action
same thing but deny resources
patrol/scout what player is doing

*/

//level 1: macro ai
//ai manages its resources -- attack groups, denying area, fortifying areas (eg resources), destroying player buildings, expanding vision
//decides on area owned (by ai), sum of the value of its units, sum of the value of the players units,
//number of resource extractors owned by player (can be included in previous sum),


//level 2: micro ai
//ai manages an individual attack group -- which one to fall back when hurt, suicide bomb, which tower/building/unit to attack
//decides based on unit health, energy, and attackpower, attackrange. enemy defence towers and resource towers have highest value
//1 squad leader per group, but 1 general/commander for the big view
// right, in which case we do path planning once and the group will follow
// or the group has a target (building, unit, etc), and they path to that as a whole?

// should we think about something like Line of sight (LOS), this is going to be local strategy I think (state machine based)
//also the commander ai will still be line of sight based as well i think, because that way it wont attack you the moment you place
//down a new building


namespace AiManager {

	int playerUnitValue = 0;
	int aiUnitValue = 0;
	int playerBuildingValue = 0;
	int aiBuildingValue = 0;

	void updateValueOfEntities() {
		aiUnitValue = 0;
		playerUnitValue = 0;
		aiBuildingValue = 0;
		playerBuildingValue = 0;
		for (auto& unit : playerUnits) {
			if (unit->aiComp.owner == GamePieceOwner::AI) {
				aiUnitValue += unit->aiComp.value;
			} else if (unit->aiComp.owner == GamePieceOwner::PLAYER) {
				playerUnitValue += unit->aiComp.value;
			}
		}

		for (auto& building : buildingMap) {
			if (building->aiComp.owner == GamePieceOwner::AI) {
				aiBuildingValue += building->aiComp.value;
			} else if (building->aiComp.owner == GamePieceOwner::PLAYER) {
				playerBuildingValue += building->aiComp.value;
			}
		}
	}

	int size = 5;

	//assume we make 1 action per frame for simplicity
	void findNextBestAction() {
		//generate tree of things to attack
//		for()
//
//		std::vector<bool> visited(size);
//		int start = 0;
//		//traverse tree with bfs
//		visited[start] = true;
//		std::queue<action> queue;
//		queue.push(start); //start from q, try to get to p, if so output yes
//		while (!queue.empty()) {
//			action u = queue.front();
//			queue.pop();
//
//			if (u == p) {
//				printf("yes\n"); //we reached p from q, meaning p is higher rated than q
//				flag = 1;
//				break; //skip the rest as we already outputted
//			}
//			for (int v : adj[u]) {
//				if (!visited[v]) {
//					visited[v] = true;
//					queue.push(v);
//				}
//			}
//		}

	}

	void update(double elapsed_ms) {
		updateValueOfEntities();

		playerUnitsSeenByAI.clear();//this is a hack because its slow, we need some removal proceedure
		aiUnitsSeenByPlayer.clear();

		for (auto& playerUnit : playerUnits) {

			for (auto& aiUnit : aiUnits) {
				if (playerUnit->canSee(aiUnit)) {
					playerUnitsSeenByAI.insert(aiUnit);
				}

				if (aiUnit->canSee(playerUnit)) {
					aiUnitsSeenByPlayer.insert(playerUnit);
				}
			}
		}
	}

	int const PRIORITIZE_CLOSER_ATTACKS = 2;

	std::shared_ptr<Entity>
	bestBuildingToAttack(std::vector<std::shared_ptr<Entity>>& buildings, Entity& entity) {
		float bestAttackValue = -1;
		std::shared_ptr<Entity> building;

		if (buildings.size() <= 0) {
			// TODO: take care of case where length of building list passed in is 0
		}

		for (auto& currentBuilding : buildings) {
			int buildingValue = currentBuilding->aiComp.value;

			float distanceToBuilding = 0;//getDistanceBetweenEntities(currentBuilding, entity); //fixme to revert

			float attackValue = buildingValue - (distanceToBuilding * PRIORITIZE_CLOSER_ATTACKS);

			if (attackValue > bestAttackValue) {
				bestAttackValue = attackValue;
				building = currentBuilding;
			}
		}

		return building;
	}

	std::shared_ptr<Entity> getHighestValuedBuilding(std::vector<std::shared_ptr<Entity>>& buildings) {
		int highestValueSoFar = -1;
		std::shared_ptr<Entity> building;

		if (buildings.size() <= 0) {
			// TODO: take care of case where length of building list passed in is 0
		}

		for (auto& currentBuilding : buildings) {
			if (currentBuilding->aiComp.value > highestValueSoFar) {
				highestValueSoFar = currentBuilding->aiComp.value;
				building = currentBuilding;
			}
		}

		return building;
	}
}