#include "pathfinder.hpp"


std::string AI::getTileNodeHashKey(tileNode &a){
	std::ostringstream result;
	result << std::get<0>(a) << "," << std::get<1>(a);
	return result.str();
}

/* using L1 Norm (Manhattan norm) for stairstep like movement, for diagonal movement 
   we can consider either LInfinity or L2 norm, leaving it for later
   we can boost performance by pre-computing those norms since the size
   of the map and location of the tiles do not change*/

float AI::l1_norm(tileNode &startNode, tileNode &nextNode){
	float rowDiff = std::abs(std::get<0>(startNode) - std::get<0>(nextNode));
	float colDiff = std::abs(std::get<1>(startNode) - std::get<1>(nextNode));
	return  rowDiff + colDiff;
}

std::vector<tileNode> AI::getNeighbors(std::vector<std::vector<tileNode>> &graph, tileNode &tile, tileNode &goal){
	size_t numOfRows = graph.size();
	size_t numOfColumns = graph[0].size();	
	long row = std::get<0>(tile);
	long col = std::get<1>(tile);	
	long goalRow = std::get<0>(goal);
	long goalCol = std::get<1>(goal);

	std::vector<tileNode> neighbors;

	/* check if we can move forward a column
	costOfForwardMove = std::get<2>(graph[row][col + 1]);
	a cost value of 1000 or larger is considered an obstacle
	that the algorithm should avoid*/
	if ((col != numOfColumns - 1) && (((row == goalRow) && (col + 1 == goalCol)) || 
		std::get<2>(graph[row][col + 1]) < 1000.0)) {
		neighbors.push_back(graph[row][col + 1]);
	}
	
	// check if we can move backward a column
	// costOfBackwardMove = std::get<2>(graph[row][col - 1]);
	if ((col != 0) && (((row == goalRow) && (col - 1 == goalCol)) ||
		std::get<2>(graph[row][col - 1]) < 1000.0)) {
		neighbors.push_back(graph[row][col - 1]);
	}

	// check if we can move up a row
	// costOfUpMove = std::get<2>(graph[row][col + 1]);
	if ((col != 0) && (((row - 1 == goalRow) && (col == goalCol)) ||
		std::get<2>(graph[row - 1][col]) < 1000.0)) {
		neighbors.push_back(graph[row - 1][col]);
	}

	// check if we can move down a row
	// costOfDownMove = std::get<2>(graph[row][col + 1]);
	if ((col != numOfRows - 1) && (((row + 1 == goalRow) && (col == goalCol)) ||
		std::get<2>(graph[row + 1][col]) < 1000.0)) {
		neighbors.push_back(graph[row + 1][col]);
	}

	return neighbors;
}

void AI::a_star(std::vector<std::vector<tileNode>>& graph, int tileSize,
	            int startx, int startz, int goalx, int goalz){

	// check which tiles the given positions lie in
	// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
	long startRow = std::floor(startx/tileSize);
	long startCol = std::floor(startz/tileSize);
	long goalRow = std::floor(startx/tileSize);
	long goalCol = std::floor(startz/tileSize);

	tileNode start = std::make_tuple(startRow, startCol, 0.0);
	tileNode goal = std::make_tuple(goalRow, goalCol, 0.0);

	frontier.push(start);
	tileNodesVisited.insert({"startingPoint", start});

	came_from[getTileNodeHashKey(start)] = start;
	cost_so_far[getTileNodeHashKey(start)] = 0;

	while (!frontier.empty()) {
		// top() does not remove the node, so call pop() after it
		tileNode current = frontier.top();
		frontier.pop();

		if (getTileNodeHashKey(current) == getTileNodeHashKey(goal)) {
			break;
		}

		std::vector<tileNode> neighbors = AI::getNeighbors(graph, current, goal);
		for (auto &next : neighbors) {
			// movement cost
			float gValue = cost_so_far[AI::getTileNodeHashKey(current)] + std::get<2>(next);
			if (gValue < cost_so_far[getTileNodeHashKey(next)]) {
				cost_so_far[next] = gValue;
				
				/*The estimate of the total cost for a path from the
				start node through this next node and onto the goal*/
				float fValue = gValue + l1_norm(next, goal);
				
			}
		}

		
	}
}
