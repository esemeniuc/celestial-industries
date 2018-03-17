#include "pathfinder.hpp"

namespace AI {
	std::vector<Coord>
	aStar::reconstruct_path(const std::unordered_map<AStarNode, AStarNode, aStarHasher>& came_from,
							const AStarNode& start,
							const AStarNode& goal) {

		AStarNode current = goal;
		std::vector<Coord> path;
		while (current != start) {
			path.emplace_back(current.colCoord, current.rowCoord);
			current = came_from.at(current); //use at() to satisfy const constraint
		}
//		path.emplace_back(start.rowCoord, start.colCoord);

		std::reverse(path.begin(), path.end());

		return path;
	}

	/* using L1 Norm (Manhattan norm) for stairstep like movement, for diagonal movement
	we can consider either L-Infinity or L2 norm, leaving it for later*/
	float aStar::l1_norm(const AStarNode& startNode, const AStarNode& goal) {
		int rowDiff = std::abs(startNode.rowCoord - goal.rowCoord);
		int colDiff = std::abs(startNode.colCoord - goal.colCoord);
		return rowDiff + colDiff;
	}

	float aStar::l2_norm(const AStarNode& startNode, const AStarNode& goal) {
		int rowDiff = startNode.rowCoord - goal.rowCoord;
		int colDiff = startNode.colCoord - goal.colCoord;
		return (float) std::sqrt((rowDiff * rowDiff) + (colDiff * colDiff));
	}

	std::vector<AStarNode>
	aStar::getNeighbors(const std::vector<std::vector<AStarNode>>& graph, AStarNode& currentPos,
						AStarNode& goal) {
		int numOfRows = (int) graph.size();
		int numOfColumns = (int) graph[0].size();
		int row = currentPos.rowCoord;
		int col = currentPos.colCoord;
		int goalRow = goal.rowCoord;
		int goalCol = goal.colCoord;

		std::vector<AStarNode> neighbors;

		/* costOfForwardMove = std::get<2>(graph[row][col + 1]);
		a cost value of 1000 or larger is considered an obstacle
		that the algorithm should avoid*/

		// check if we can move forward a column
		const float OBSTACLE_COST = 1000.0;
		if ((col != numOfColumns - 1) && (((row == goalRow) && (col + 1 == goalCol)) ||
										  graph[row][col + 1].movementCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row][col + 1]);
		}

		// check if we can move backward a column
		// costOfBackwardMove = std::get<2>(graph[row][col - 1]);
		if ((col != 0) && (((row == goalRow) && (col - 1 == goalCol)) ||
						   graph[row][col - 1].movementCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row][col - 1]);
		}

		// check if we can move up a row
		// costOfUpMove = std::get<2>(graph[row][col + 1]);
		if ((row != 0) && (((row - 1 == goalRow) && (col == goalCol)) ||
						   graph[row - 1][col].movementCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row - 1][col]);
		}

		// check if we can move down a row
		// costOfDownMove = std::get<2>(graph[row][col + 1]);
		if ((row != numOfRows - 1) && (((row + 1 == goalRow) && (col == goalCol)) ||
									   graph[row + 1][col].movementCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row + 1][col]);
		}

		return neighbors;
	}

	//returns a pair indicating whether the path was found, and the path itself
	std::pair<bool, std::vector<Coord>>
	AI::aStar::a_star(const std::vector<std::vector<AStarNode>>& graph,
					  int tileSize, int startX, int startZ, int goalX, int goalZ) {

		/* a min heap that will store nodes we have not explored yet in the level map
		will use it to fetch the tile node with the smallest f-score*/
		std::priority_queue<AStarNode, std::vector<AStarNode>, Compare> frontier;

		/* hash table that helps us keep track of how we reached a tile node
		using a key of its predecessor node in the path*/
		std::unordered_map<AStarNode, AStarNode, aStarHasher> came_from;
		came_from.reserve(graph.size() * graph.front().size());

		// cost associated with a path up to a certain node
		std::unordered_map<AStarNode, double, aStarHasher> cost_so_far;
		cost_so_far.reserve(graph.size() * graph.front().size());

		// check which tiles the given positions lie in
		// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
		int startRow = startX / tileSize; //floating point numbers get floored when stored in ints
		int startCol = startZ / tileSize;
		int goalRow = goalX / tileSize;
		int goalCol = goalZ / tileSize;

		AStarNode start = AStarNode(startCol, startRow, 10, 0);
		AStarNode goal = AStarNode(goalCol, goalRow, 10, INF);

		frontier.push(start);
		came_from[start] = start;
		cost_so_far[start] = 0.0;

		while (!frontier.empty()) {
			// top() does not remove the node, so call pop() after it
			AStarNode current = frontier.top();
			frontier.pop();

			if (current == goal) {
				auto path = reconstruct_path(came_from, start, goal);
				logger(LogLevel::INFO) << "Found path with length " << path.size() << " \n";
				return {true, path}; //true for bool because we found a path
			}

			std::vector<AStarNode> neighbors = aStar::getNeighbors(graph, current, goal);
			for (auto& next : neighbors) {
				// total movement cost to next node: path cost of current node + cost of taking 
				// a step from current to next node
				float gScore = (float) cost_so_far[current] + next.movementCost;
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
