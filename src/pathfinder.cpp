#include "pathfinder.hpp"
#include "global.hpp" //for ai cost map

namespace AI {
	namespace aStar {

		//returns true if a spot can be traversed upon
		bool isTraversable(int x, int z) {
			return Global::levelWithUnitsTraversalCostMap[z][x].movementCost < Config::OBSTACLE_COST;
		}

		std::vector<glm::vec3> reconstruct_path(const std::unordered_map<AStarNode, AStarNode, aStarHasher>& came_from,
												const AStarNode& startNode, const AStarNode& goalNode,
												const glm::vec3& startPos, const glm::vec3& goalPos) {
			AStarNode current = goalNode;
			std::vector<glm::vec3> path;
			path.push_back(goalPos); //add the end as we need to interpolate from aStar int to last movement

			while (current != startNode) {
				path.emplace_back(current.colCoord, 0, current.rowCoord);
				current = came_from.at(current); //use at() to satisfy const constraint
			}
			path.push_back(startPos); //add the start as we need to interpolate start to first movement

			std::reverse(path.begin(), path.end());
			return path;
		}

		/* using L1 Norm (Manhattan norm) for stairstep like movement, for diagonal movement
		we can consider either L-Infinity or L2 norm, leaving it for later*/
		float l1_norm(const AStarNode& startNode, const AStarNode& goal) {
			int rowDiff = std::abs(startNode.rowCoord - goal.rowCoord);
			int colDiff = std::abs(startNode.colCoord - goal.colCoord);
			return rowDiff + colDiff;
		}

		float l2_norm(const AStarNode& startNode, const AStarNode& goal) {
			int rowDiff = startNode.rowCoord - goal.rowCoord;
			int colDiff = startNode.colCoord - goal.colCoord;
			return sqrtf((rowDiff * rowDiff) + (colDiff * colDiff));
		}

		bool isGoalOrCheaper(int nextCol, int nextRow, int goalCol, int goalRow,
							 const std::vector<std::vector<AStarNode>>& graph) {
			return (nextCol == goalCol && nextRow == goalRow) ||
				   graph[nextRow][nextCol].movementCost < Config::OBSTACLE_COST;
		}

		bool withinLevelBounds(int col, int row) {
			return (col >= 0 && col < (int)Global::levelWidth) &&
				   (row >= 0 && row < (int)Global::levelHeight);
		}

		std::vector<AStarNode>
		getNeighbors(const std::vector<std::vector<AStarNode>>& graph, const AStarNode& currentPos,
					 const AStarNode& goal) {
			int numOfRows = (int) graph.size();
			int numOfColumns = (int) graph.front().size();
			int row = currentPos.rowCoord;
			int col = currentPos.colCoord;
			int goalRow = goal.rowCoord;
			int goalCol = goal.colCoord;
			std::vector<AStarNode> neighbors;

			/* costOfForwardMove = std::get<2>(graph[row][col + 1]);
			a cost value of 1000 or larger is considered an obstacle
			that the algorithm should avoid*/
			if (col < 0 || col >= numOfColumns || row < 0 || col >= numOfRows) {
				logger(LogLevel::ERR) << "ENTITY PATHING FROM OUT OF LEVEL \n";
				throw "ENTITY PATHING FROM OUT OF LEVEL";
			}

			int nextRow, nextCol;
			for (const auto& dir : directions) {
				nextCol = currentPos.colCoord + dir.first, nextRow = currentPos.rowCoord + dir.second;
				if (withinLevelBounds(nextCol, nextRow) &&
					isGoalOrCheaper(nextCol, nextRow, goalCol, goalRow, graph)) {
					neighbors.push_back(graph[nextRow][nextCol]);
				}
			}

			return neighbors;
		}

		//returns a pair indicating whether the path was found, and the path itself
		std::pair<bool, std::vector<glm::vec3>>
		findPath(const glm::vec3& start, const glm::vec3& goal, int tileSize) {

			/* a min heap that will store nodes we have not explored yet in the level map
			will use it to fetch the tile node with the smallest f-score*/
			std::priority_queue<AStarNode, std::vector<AStarNode>, aStarComparator> frontier;

			/* hash table that helps us keep track of how we reached a tile node
			using a key of its predecessor node in the path*/
			std::unordered_map<AStarNode, AStarNode, aStarHasher> came_from;
			came_from.reserve(Global::levelWidth * Global::levelHeight);

			// cost associated with a path up to a certain node
			std::unordered_map<AStarNode, double, aStarHasher> cost_so_far;
			cost_so_far.reserve(Global::levelWidth * Global::levelHeight);

			// check which tiles the given positions lie in
			// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
			int startRow = int((start.z + 0.5) / tileSize); //floating point numbers get floored when stored in ints
			int startCol = int((start.x + 0.5) / tileSize);
			int goalRow = int((goal.z + 0.5) / tileSize);
			int goalCol = int((goal.x + 0.5) / tileSize);

			AStarNode startNode = AStarNode(startCol, startRow, Config::DEFAULT_TRAVERSABLE_COST,
											0.0f, Model::MeshType::NONE);
			AStarNode goalNode = AStarNode(goalCol, goalRow, Config::DEFAULT_TRAVERSABLE_COST,
										   INF, Model::MeshType::NONE);

			//required to use this because of hashing being weird on floats

			frontier.push(startNode);
			came_from[startNode] = startNode;
			cost_so_far[startNode] = 0.0;

			while (!frontier.empty()) {
				// top() does not remove the node, so call pop() after it
				AStarNode current = frontier.top();
				frontier.pop();

				if (current == goalNode) {
					auto path = reconstruct_path(came_from, startNode, goalNode, start, goal);
					// logger(LogLevel::INFO) << "Found path with length " << path.size() << " \n";
					return {true, path}; //true for bool because we found a path
				}

				std::vector<AStarNode> neighbors = getNeighbors(Global::aStarCostMap, current, goalNode);
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
						next.fScore = gScore + l2_norm(next, goalNode);
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
