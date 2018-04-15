#include "pathfinder.hpp"
#include "global.hpp" //for ai cost map

namespace AI {
	namespace aStar {

		//returns true if a spot can be traversed upon
		bool isTraversable(int x, int z) {
			return Global::levelWithUnitsTraversalCostMap[z][x].movementCost < Config::OBSTACLE_COST;
		}

		std::vector<glm::vec3> reconstruct_path(const std::unordered_map<AStarNode, AStarNode, aStarHasher>& came_from,
												const AStarNode& start, const AStarNode& goal) {
			AStarNode current = goal;
			std::vector<glm::vec3> path;
			path.emplace_back(goal.colCoord, 0, goal.rowCoord); //add the start as we need to interpolate start to first movement

			while (current != start) {
				path.emplace_back(current.colCoord, 0, current.rowCoord);
				current = came_from.at(current); //use at() to satisfy const constraint
			}
			path.emplace_back(start.colCoord, 0, start.rowCoord); //add the start as we need to interpolate start to first movement

			std::reverse(path.begin(), path.end());
			return path;
		}

		/* using L1 Norm (Manhattan norm) for stairstep like movement, for diagonal movement
		we can consider either L-Infinity or L2 norm, leaving it for later*/
		float l1_norm(const AStarNode& startNode, const AStarNode& goal) {
			float rowDiff = std::abs(startNode.rowCoord - goal.rowCoord);
			float colDiff = std::abs(startNode.colCoord - goal.colCoord);
			return rowDiff + colDiff;
		}

		float l2_norm(const AStarNode& startNode, const AStarNode& goal) {
			float rowDiff = startNode.rowCoord - goal.rowCoord;
			float colDiff = startNode.colCoord - goal.colCoord;
			return sqrtf((rowDiff * rowDiff) + (colDiff * colDiff));
		}

		std::vector<AStarNode>
		getNeighbors(const std::vector<std::vector<AStarNode>>& graph, const AStarNode& currentPos,
					 const AStarNode& goal) {
			int numOfRows = (int) graph.size();
			int numOfColumns = (int) graph.front().size();
			int row = int(currentPos.rowCoord + 0.5); //need to round since floats will cause the hashing to not work
			int col = int(currentPos.colCoord + 0.5);
			int goalRow = int(goal.rowCoord + 0.5);
			int goalCol = int(goal.colCoord + 0.5);

			std::vector<AStarNode> neighbors;

			/* costOfForwardMove = std::get<2>(graph[row][col + 1]);
			a cost value of 1000 or larger is considered an obstacle
			that the algorithm should avoid*/
			if (col < 0 || col >= numOfColumns || row < 0 || col >= numOfRows) {
				logger(LogLevel::ERR) << "ENTITY PATHING FROM OUT OF LEVEL \n";
				throw "ENTITY PATHING FROM OUT OF LEVEL";
			}

			// check if we can move forward a column
			if ((col < numOfColumns - 1) &&
				(((row == goalRow) && (col + 1 == goalCol)) ||
				 graph[row][col + 1].movementCost < Config::OBSTACLE_COST)) {
				neighbors.push_back(graph[row][col + 1]);
			}

			// check if we can move backward a column
			// costOfBackwardMove = std::get<2>(graph[row][col - 1]);
			if ((col > 0) &&
				(((row == goalRow) && (col - 1 == goalCol)) ||
				 graph[row][col - 1].movementCost < Config::OBSTACLE_COST)) {
				neighbors.push_back(graph[row][col - 1]);
			}

			// check if we can move up a row
			// costOfUpMove = std::get<2>(graph[row][col + 1]);
			if ((row > 0) &&
				(((row - 1 == goalRow) && (col == goalCol)) ||
				 graph[row - 1][col].movementCost < Config::OBSTACLE_COST)) {
				neighbors.push_back(graph[row - 1][col]);
			}

			// check if we can move down a row
			// costOfDownMove = std::get<2>(graph[row][col + 1]);
			if ((row < numOfRows - 1) &&
				(((row + 1 == goalRow) && (col == goalCol)) ||
				 graph[row + 1][col].movementCost < Config::OBSTACLE_COST)) {
				neighbors.push_back(graph[row + 1][col]);
			}

			return neighbors;
		}

		//returns a pair indicating whether the path was found, and the path itself
		std::pair<bool, std::vector<glm::vec3>>
		findPath(float startX, float startZ, float goalX, float goalZ, int tileSize) {

			/* a min heap that will store nodes we have not explored yet in the level map
			will use it to fetch the tile node with the smallest f-score*/
			std::priority_queue<AStarNode, std::vector<AStarNode>, aStarComparator> frontier;

			/* hash table that helps us keep track of how we reached a tile node
			using a key of its predecessor node in the path*/
			std::unordered_map<AStarNode, AStarNode, aStarHasher> came_from;
			came_from.reserve(Global::levelArray.size() * Global::levelArray.front().size());

			// cost associated with a path up to a certain node
			std::unordered_map<AStarNode, double, aStarHasher> cost_so_far;
			cost_so_far.reserve(Global::levelArray.size() * Global::levelArray.front().size());

			// check which tiles the given positions lie in
			// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
			float startRow = startZ / tileSize; //floating point numbers get floored when stored in ints
			float startCol = startX / tileSize;
			float goalRow = goalZ / tileSize;
			float goalCol = goalX / tileSize;

			AStarNode start = AStarNode(startCol, startRow, Config::DEFAULT_TRAVERSABLE_COST,
										0.0f, Model::MeshType::NONE);
			AStarNode goal = AStarNode(goalCol, goalRow, Config::DEFAULT_TRAVERSABLE_COST,
									   INF, Model::MeshType::NONE);

			//required to use this because of hashing being weird on floats

			AStarNode startRounded = AStarNode(roundf(startCol), roundf(startRow), Config::DEFAULT_TRAVERSABLE_COST,
											   0.0f, Model::MeshType::NONE);

			AStarNode goalRounded = AStarNode(roundf(goalCol), roundf(goalRow), Config::DEFAULT_TRAVERSABLE_COST,
											  INF, Model::MeshType::NONE);
			frontier.push(startRounded);
			came_from[startRounded] = startRounded;
			cost_so_far[startRounded] = 0.0;

			while (!frontier.empty()) {
				// top() does not remove the node, so call pop() after it
				AStarNode current = frontier.top();
				frontier.pop();

				if (current == goalRounded) {
					auto path = reconstruct_path(came_from, start, goal);
					logger(LogLevel::INFO) << "Found path with length " << path.size() << " \n";
					return {true, path}; //true for bool because we found a path
				}

				std::vector<AStarNode> neighbors = getNeighbors(Global::aStarCostMap, current, goalRounded);
				for (auto& next : neighbors) {
					// total movement cost to next node: path cost of current node + cost of taking
					// a step from current to next node
					float gScore = float(cost_so_far[current] + next.movementCost);
					// if a neighbor node was not explored before, or we found a new path to it
					// that has a lower cost
					if (cost_so_far.count(next) == 0 || gScore < cost_so_far[next]) {
						// if neighbor node was found in the table update it
						// if not, the [] operator will insert a new entry
						cost_so_far[next] = gScore;
						// calculate f-score based on g-score and heuristic
						next.fScore = gScore + l1_norm(next, goal);
						// add neighbor to open list of nodes to explore
						frontier.push(next);
						// update predecessor of next node to our current node
						came_from[next] = current;
					}
				}
			}

			return {false, {}}; //false for bool because we didn't find a path
		}
	}
}
