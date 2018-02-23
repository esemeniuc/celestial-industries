#include "pathfinder.hpp"

namespace AI {
	std::string aStar::getTileNodeHashKey(const aStarPathState &a) {
		std::ostringstream result;
		result << a.rowCoord << "," << a.colCoord;
		return result.str();
	}

	std::vector<aStarPathState> aStar::reconstruct_path(std::unordered_map<std::string, aStarPathState> &came_from,
														const aStarPathState &start,
														const aStarPathState &goal) {
		aStarPathState current = goal;
		std::vector<aStarPathState> path;
		while (getTileNodeHashKey(current) != getTileNodeHashKey(start)) {
			path.push_back(current);
			current = came_from[getTileNodeHashKey(current)];
		}
		std::reverse(path.begin(), path.end());
		return path;
	}

	/* using L1 Norm (Manhattan norm) for stairstep like movement, for diagonal movement
	we can consider either L-Infinity or L2 norm, leaving it for later*/
	float aStar::l1_norm(aStarPathState &startNode, aStarPathState &goal) {
		float rowDiff = std::abs(startNode.rowCoord - goal.rowCoord);
		float colDiff = std::abs(startNode.colCoord - goal.colCoord);
		return  rowDiff + colDiff;
	}

	std::vector<aStarPathState> aStar::getNeighbors(std::vector<std::vector<aStarPathState>> &graph, aStarPathState &tile, aStarPathState &goal) {
		size_t numOfRows = graph.size();
		size_t numOfColumns = graph[0].size();
		long row = tile.rowCoord;
		long col = tile.colCoord;
		long goalRow = goal.rowCoord;
		long goalCol = goal.colCoord;

		std::vector<aStarPathState> neighbors;

		/* costOfForwardMove = std::get<2>(graph[row][col + 1]);
		a cost value of 1000 or larger is considered an obstacle
		that the algorithm should avoid*/

		// check if we can move forward a column
		const float OBSTACLE_COST = 1000.0;
		if ((col != (long)numOfColumns - 1) && (((row == goalRow) && (col + 1 == goalCol)) ||
		  	graph[row][col + 1].currentPathCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row][col + 1]);
		}

		// check if we can move backward a column
		// costOfBackwardMove = std::get<2>(graph[row][col - 1]);
		if ((col != 0) && (((row == goalRow) && (col - 1 == goalCol)) ||
			graph[row][col - 1].currentPathCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row][col - 1]);
		}

		// check if we can move up a row
		// costOfUpMove = std::get<2>(graph[row][col + 1]);
		if ((col != 0) && (((row - 1 == goalRow) && (col == goalCol)) ||
			graph[row - 1][col].currentPathCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row - 1][col]);
		}

		// check if we can move down a row
		// costOfDownMove = std::get<2>(graph[row][col + 1]);
		if ((col != (long)numOfRows - 1) && (((row + 1 == goalRow) && (col == goalCol)) ||
			graph[row + 1][col].currentPathCost < OBSTACLE_COST)) {
			neighbors.push_back(graph[row + 1][col]);
		}

		return neighbors;
	}

	std::pair<bool, std::vector<aStarPathState>> AI::aStar::a_star(std::vector<std::vector<aStarPathState>> graph, int tileSize,
															 int startx, int startz, int goalx, int goalz) {

		bool pathFound = false;

		/* a min heap that will store nodes we have not explored yet in the level map
		will use it to fetch the tile node with the smallest f-score*/
		std::priority_queue<aStarPathState, std::vector<aStarPathState>, Compare> frontier;

		/* hash table that helps us keep track of how we reached a tile node
		using a key of its predecessor node in the path*/
		std::unordered_map<std::string, aStarPathState> came_from;

		// cost associated with a path up to a certain node
		std::unordered_map<std::string, double> cost_so_far;

		// check which tiles the given positions lie in
		// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
		long startRow = std::floor(startx / tileSize);
		long startCol = std::floor(startz / tileSize);
		long goalRow = std::floor(goalx / tileSize);
		long goalCol = std::floor(goalz / tileSize);

		aStarPathState start = aStarPathState(startRow, startCol, 10, 0.0);
		aStarPathState goal = aStarPathState(goalRow, goalCol, 10, INF);

		frontier.push(start);
		came_from[getTileNodeHashKey(start)] = start;
		cost_so_far[getTileNodeHashKey(start)] = 0.0;

		while (!frontier.empty()) {
			// top() does not remove the node, so call pop() after it
			aStarPathState current = frontier.top();
			frontier.pop();

			if (getTileNodeHashKey(current) == getTileNodeHashKey(goal)) {
				pathFound = true;
				break;
			}

			std::vector<aStarPathState> neighbors = aStar::getNeighbors(graph, current, goal);
			for (auto &next : neighbors) {
				// total movement cost to next node: path cost of current node + cost of taking 
				// a step from current to next node
				float gScore = cost_so_far[getTileNodeHashKey(current)] + next.currentPathCost;
				// if a neighbor node was not explored before, or we found a new path to it
				// that has a lower cost
				if (cost_so_far.find(getTileNodeHashKey(next)) == cost_so_far.end() ||
					gScore < cost_so_far[getTileNodeHashKey(next)]) {
					// if neighbor node was found in the table update it
					// if not, the [] operator will insert a new entry
					cost_so_far[getTileNodeHashKey(next)] = gScore;
					// calculate f-score based on g-score and heuristic
					float fScore = gScore + l1_norm(next, goal);
					next.fScore = fScore;
					// add neighbor to open list of nodes to explore
					frontier.push(next);
					// update predecessor of next node to our current node
					came_from[getTileNodeHashKey(next)] = current;
				}
			}
		}
		std::vector<aStarPathState> result = reconstruct_path(came_from, start, goal);
		return {pathFound, result};
	}
}