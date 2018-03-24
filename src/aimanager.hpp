//
// Created by eric on 2/25/18.
//

#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>

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

//to do
//make scouting value function
//make visibility map
//calculate how much is seen
//want 50% of area seen in last 10 secs



//make deny value function

//make attack value function


namespace AiManager {

	int playerUnitValue = 0;
	int aiUnitValue = 0;
	int playerBuildingValue = 0;
	int aiBuildingValue = 0;
	double percentVisible = 0;
	const double AI_VISIBLE_THRESHOLD = 0.5; //scout only if more than this value
	const int AI_RUN_THRESHOLD = 1000; //run every 1000ms
	const int DIST_THRESHOLD = 6;
	double lastRunTimestamp = AI_RUN_THRESHOLD;

	std::vector<Coord> scoutingTargetsInProgress;


	int aiSpawnX, aiSpawnZ;

	std::vector<std::vector<uint32_t>> visibilityMap; //stores the last seen time of each cell by ai

	void init(size_t levelHeight, size_t levelWidth) {
		visibilityMap = std::vector<std::vector<uint32_t>>(levelHeight, std::vector<uint32_t>(levelWidth));
		aiSpawnX = levelWidth - 1;//FIXME: a hack to make ai always spawn from far right of the map
		aiSpawnZ = levelHeight / 2;
	}


	void updateVisibilityMap() {
		//FIXME:hacky to just reset every time
		visibilityMap = std::vector<std::vector<uint32_t>>(world.levelHeight, std::vector<uint32_t>(world.levelWidth));

		int cellsVisible = 0;
		for (auto& unit : aiUnits) {
			size_t radius = (size_t) unit->aiComp.visionRange;
			size_t xMin = std::max<size_t>(0, (size_t) unit->getPositionInt().colCoord - radius);
			size_t xMax = std::min<size_t>(world.levelWidth, (size_t) unit->getPositionInt().colCoord + radius);
			size_t yMin = std::max<size_t>(0, (size_t) unit->getPositionInt().colCoord - radius);
			size_t yMax = std::min<size_t>(world.levelWidth, (size_t) unit->getPositionInt().colCoord + radius);

			for (size_t i = yMin; i < yMax; ++i) {
				for (size_t j = xMin; j < xMax; ++j) {
					if (i * i + j * j <= radius * radius && visibilityMap[i][j] == 0) {
						visibilityMap[i][j] = 1;//we can see it
						cellsVisible++;
					}
				}
			}
		}

		percentVisible = (double) cellsVisible / (world.levelHeight * world.levelWidth);
	}

	//calculates the values of ai and player units and buildings
	void updateValueOfEntities() {
		aiUnitValue = 0;
		playerUnitValue = 0;
		aiBuildingValue = 0;
		playerBuildingValue = 0;
		for (auto& unit : playerUnits) {
			playerUnitValue += unit->aiComp.value;
		}

		for (auto& unit : aiUnits) {
			aiUnitValue += unit->aiComp.value;
		}

		for (auto& building : buildingMap) {
			if (building->aiComp.owner == GamePieceOwner::AI) {
				aiBuildingValue += building->aiComp.value;
			} else if (building->aiComp.owner == GamePieceOwner::PLAYER) {
				playerBuildingValue += building->aiComp.value;
			}
		}
	}

	//returns the number of newly found cells during scouting by placing a unit at (x,z)
	int getNumberOfNewCellsIfScout(int x, int y, int radius) {
		int newCellsFound = 0;
		for (auto& unit : aiUnits) {
			int xMin = std::max(0, x - radius);
			int xMax = std::min((int) world.levelWidth, x + radius);
			int yMin = std::max(0, y - radius);
			int yMax = std::min((int) world.levelHeight, y + radius);

			for (int i = yMin; i < yMax; ++i) {
				for (int j = xMin; j < xMax; ++j) {
					if (i * i + j * j <= radius * radius && visibilityMap[i][j] == 0) {
						newCellsFound++;
					}
				}
			}
		}

		return newCellsFound;
	}

	struct bfsState {
		int x, z, unseenDistance;

		bfsState(int x, int z, int unseenDistance) : x(x), z(z), unseenDistance(unseenDistance) {}
	};


	bool isWithinRange(int x, int z) {
		return x >= 0 && x < world.levelWidth &&
			   z >= 0 && z < world.levelHeight;
	}

	//in x,z format
	std::pair<int, int> adj[4] = {{0,  1}, //down
								  {0,  -1}, //up
								  {1,  0}, //right
								  {-1, 0}//left
	};

	bool withinRangeOfOtherScoutTargets(int x, int z) {
		for (const auto& existingTarget : scoutingTargetsInProgress) {
			glm::vec3 newTarget(x, 0, z);
			glm::vec3 existingTargetV(existingTarget.colCoord, 0, existingTarget.rowCoord);
			float dist = glm::l2Norm(existingTargetV, newTarget);
			if (dist < DIST_THRESHOLD) {
				return true;
			}
		}
		return false;
	}

	//assume we make 1 action per frame for simplicity, assume vision range is fixed
	Coord findBestScoutLocation() {
		std::vector<std::vector<bool>> visited(world.levelHeight, std::vector<bool>(world.levelWidth));
		std::vector<std::vector<std::pair<int, int>>> parent(world.levelHeight,
															 std::vector<std::pair<int, int>>(world.levelWidth));
		//traverse tree with bfs
		std::pair<int, int> root(-1, -1);
		parent[aiSpawnZ][aiSpawnX] = root;
		visited[aiSpawnZ][aiSpawnX] = true;
		std::queue<bfsState> queue;
		queue.push({aiSpawnX, aiSpawnZ, 0});
		while (!queue.empty()) {
			bfsState u = queue.front();
			queue.pop();

			if (u.unseenDistance >= DIST_THRESHOLD && !withinRangeOfOtherScoutTargets(u.x, u.z)) {
				scoutingTargetsInProgress.emplace_back(u.x, u.z);
				std::pair<int, int> traverser(u.x, u.z);
				for (int i = DIST_THRESHOLD; i > DIST_THRESHOLD / 2 && traverser != root; i--)//get to center of circle
				{
					traverser = parent[traverser.second][traverser.first]; //parent[z][x]
				}
				return Coord(traverser.first, traverser.second);
			}

			for (std::pair<int, int> dir : adj) {
				int nextX = u.x + dir.first;
				int nextZ = u.z + dir.second;
				int nextDist = u.unseenDistance;
				if (isWithinRange(nextX, nextZ) && !visited[nextZ][nextX]) {
					visited[nextZ][nextX] = true;
					parent[nextZ][nextX] = {u.x, u.z};
					if (visibilityMap[nextZ][nextX] > 0) {
						nextDist++;//FIXME: change this to be relative later
					}
					queue.push({nextX, nextZ, nextDist});
				}
			}
		}

		//assumes this is the player spawn and just goes to it
		int playerSpawnX = 1;
		int playerSpawnZ = world.levelHeight / 2;
		scoutingTargetsInProgress.emplace_back(playerSpawnX, playerSpawnZ);
		return Coord(playerSpawnX, playerSpawnZ);
	}

//updates the state trackers of what units the ai has seen
	void updateUnitsSeen() {
		playerUnitsSeenByAI.clear();//this is a hack because its slow, we need some removal proceedure
		aiUnitsSeenByPlayer.clear();

		for (const auto& playerUnit : playerUnits) {

			for (const auto& aiUnit : aiUnits) {
				if (playerUnit->canSee(aiUnit)) {
					playerUnitsSeenByAI.insert(aiUnit);
				}

				if (aiUnit->canSee(playerUnit)) {
					aiUnitsSeenByPlayer.insert(playerUnit);
				}
			}
		}
	}

	std::shared_ptr<Entity> getBestScoutUnit(const Coord& targetLocation) {
		glm::vec3 targetLocation3(targetLocation.colCoord, 0, targetLocation.rowCoord);

		std::shared_ptr<Entity> bestUnit;
		float bestDist = std::numeric_limits<float>::max();
		for (const auto& unit : aiUnits) {
			float dist = glm::l2Norm(targetLocation3, unit->getPosition());
			if (dist < bestDist && unit->unitComp.state == UnitState::IDLE) {
				bestUnit = unit;
				bestDist = dist;
			}
		}
		return bestUnit;
	}

	void update(double elapsed_ms) {
		lastRunTimestamp += elapsed_ms;

		if (lastRunTimestamp > AI_RUN_THRESHOLD) {
			lastRunTimestamp = 0; //reset as we past the threshold
		} else {
			return; //run only after we exceed the threshold
		}

		updateValueOfEntities();
		updateUnitsSeen();

		//send unit to scout
		updateVisibilityMap();
		if (percentVisible < AI_VISIBLE_THRESHOLD) {
			Coord loc = findBestScoutLocation();
			std::shared_ptr<Entity> bestUnit = getBestScoutUnit(loc);
			if (bestUnit) { //if not null
				bestUnit->scoutPosition(loc.colCoord, loc.rowCoord);
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