//
// Created by eric on 2/25/18.
//

#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/norm.hpp>

#include "common.hpp"
#include "global.hpp"
#include "unitcomp.hpp"
#include "building.hpp"

#include <array>

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
	const double AI_VISIBLE_THRESHOLD = 0.5; //scout only if we've seen less than this value
	const int AI_RUN_THRESHOLD = 500; //run every 500ms
	const int UNSEEN_RADIUS_THRESHOLD = 6;
	double lastRunTimestamp = AI_RUN_THRESHOLD;
	const int FOG_OF_WAR_TIME_THRESHOLD = 10;

	std::unordered_set<Coord, CoordHasher> scoutingTargetsInProgress;


	int aiSpawnX, aiSpawnZ;

	void init(size_t levelHeight, size_t levelWidth) {
		aiVisibilityMap = std::vector<std::vector<int>>(levelHeight, std::vector<int>(levelWidth));
		aiSpawnX = levelWidth - 1;//FIXME: a hack to make ai always spawn from far right of the map
		aiSpawnZ = levelHeight / 2;
	}


	void updateAreaSeenByUnit(const std::shared_ptr<Entity>& unit, int currentUnixTime,
							  std::vector<std::vector<int>> visibilityMap) {
		int radius = unit->aiComp.visionRange;
		int xMin = std::max(0, unit->getPositionInt().colCoord - radius);
		int xMax = std::min((int) world.levelWidth, unit->getPositionInt().colCoord + radius);
		int yMin = std::max(0, unit->getPositionInt().rowCoord - radius);
		int yMax = std::min((int) world.levelHeight, unit->getPositionInt().rowCoord + radius);

		for (int i = yMin; i < yMax; ++i) {
			for (int j = xMin; j < xMax; ++j) {
				int xp = i - unit->getPositionInt().colCoord;
				int yp = j - unit->getPositionInt().rowCoord;
				if (xp * xp + yp * yp <= radius * radius) {
					visibilityMap[i][j] = currentUnixTime;
				}
			}
		}
	}

	void updateVisibilityMap() {
		int currentUnixTime = (int) getUnixTime();
		for (const auto& unit : aiUnits) {
			updateAreaSeenByUnit(unit, currentUnixTime, aiVisibilityMap);
		}

		int cellsVisible = 0;
		for (const auto& row: aiVisibilityMap) {
			for (const auto& cellLastSceneTime : row) {
				if (currentUnixTime - cellLastSceneTime < FOG_OF_WAR_TIME_THRESHOLD) {
					cellsVisible++;
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


	struct bfsState {
		int x, z, unseenDistance, unseenTimeDeltaSum;
		//unseenTimeDeltaSum is to prioritize areas that haven't been seen in a long time

		bfsState(int x, int z, int unseenDistance, int unseenTimeDeltaSum) : x(x), z(z), unseenDistance(unseenDistance),
																			 unseenTimeDeltaSum(unseenTimeDeltaSum) {}

		bool operator<(const bfsState& rhs) const { //prioritizes the ones that have the most unseen area
			return unseenTimeDeltaSum < rhs.unseenTimeDeltaSum;
		}
	};


	bool isWithinBounds(int x, int z) {
		return x >= 0 && x < (int) world.levelWidth &&
			   z >= 0 && z < (int) world.levelHeight;
	}

	//in x,z format
	std::array<std::pair<int, int>, 4> adj = {{{0, 1}, //down
													  {0, -1}, //up
													  {1, 0}, //right
													  {-1, 0} //left
											  }};

	bool withinRangeOfOtherScoutTargets(int x, int z) {
		for (const auto& existingTarget : scoutingTargetsInProgress) {
			glm::vec3 newTarget(x, 0, z);
			glm::vec3 existingTargetV(existingTarget.colCoord, 0, existingTarget.rowCoord);
			float dist = glm::l2Norm(existingTargetV, newTarget);
			if (dist < 2 * UNSEEN_RADIUS_THRESHOLD) {
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
		int currentUnixTime = static_cast<int>(getUnixTime());
		std::pair<int, int> root(INT32_MIN, INT32_MIN);
		parent[aiSpawnZ][aiSpawnX] = root;
		visited[aiSpawnZ][aiSpawnX] = true;
		std::priority_queue<bfsState> queue;
		queue.push({aiSpawnX, aiSpawnZ, 0, 0});
		while (!queue.empty()) {
			bfsState u = queue.top();
			queue.pop();

			//use 2x because we want to capture diameter unseen dist
			if (u.unseenDistance >= 2 * UNSEEN_RADIUS_THRESHOLD && !withinRangeOfOtherScoutTargets(u.x, u.z)) {
				std::pair<int, int> traverser(u.x, u.z);
				//get to center of circle
				for (int i = UNSEEN_RADIUS_THRESHOLD; i > UNSEEN_RADIUS_THRESHOLD / 2 && traverser != root; i--) {
					traverser = parent[traverser.second][traverser.first]; //parent[z][x]
				}
				return Coord(traverser.first, traverser.second);
			}

			for (std::pair<int, int> dir : adj) {
				int nextX = u.x + dir.first;
				int nextZ = u.z + dir.second;
				int nextDist = u.unseenDistance;
				if (isWithinBounds(nextX, nextZ) && !visited[nextZ][nextX] && AI::aStar::isTraversable(nextX, nextZ)) {
					visited[nextZ][nextX] = true;
					parent[nextZ][nextX] = {u.x, u.z};
					const int timeDelta = currentUnixTime - aiVisibilityMap[nextZ][nextX];
					if (timeDelta > FOG_OF_WAR_TIME_THRESHOLD) {
						nextDist++;
					}
					queue.push({nextX, nextZ, nextDist, u.unseenTimeDeltaSum + timeDelta});
				}
			}
		}

		//assumes this is the player spawn and just goes to it
		Coord randomCoord = Coord::getRandomCoord(world.levelWidth, world.levelHeight);
//		while(AI::aStar::a_star(aStarCostMap, 1, randomCoord.colCoord, randomCoord.rowCoord, x,
//								z).second))
		//check if traversable


//		scoutingTargetsInProgress.emplace_back(playerSpawnX, playerSpawnZ);
		return randomCoord;
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

	void cleanupScoutTargets() {
		for (const auto& aiUnit: aiUnits) {
			if (aiUnit->unitComp.state == UnitState::SCOUT && !aiUnit->hasTarget()) //check for finish scouting
			{
//				scoutingTargetsInProgress.erase()
			}
		}
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

		//print grid
//		for (const auto& row: aiVisibilityMap) {
//			for (const auto& cellLastSceneTime : row) {
//				printf("%d ", cellLastSceneTime);
//			}
//			printf("\n");
//		}
		if (percentVisible < AI_VISIBLE_THRESHOLD) {
			Coord loc = findBestScoutLocation();
			std::shared_ptr<Entity> bestUnit = getBestScoutUnit(loc);
			if (bestUnit) { //if not null
				bestUnit->scoutPosition(loc.colCoord, loc.rowCoord);
				scoutingTargetsInProgress.insert(loc);
			}
		}
	}
}