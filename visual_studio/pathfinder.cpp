#include <limits>
#include "pathfinder.hpp"


std::string AI::getTileNodeHashKey(tileNode & a){
	std::ostringstream result;
	result << std::get<0>(a) << "," << std::get<1>(a);
	return result.str();
}

float AI::l1_norm(tileNode & a, tileNode & b){
	float rowDiff = std::abs(std::get<0>(a) - std::get<0>(b));
	float colDiff = std::abs(std::get<1>(a) - std::get<1>(b));
	return  rowDiff + colDiff;
}

std::vector<tileNode> AI::getNeighbors(tileNode & a){

	return std::vector<tileNode>();
}

void AI::a_star(std::vector<std::vector<tileNode>>& graph, int tileSize,
	            int startx, int startz, int goalx, int goalz){

	// check which tiles the given positions lie in
	// TODO: check coordinate signs ( -Z as opposed to +Z for tile positions)
	long startRow = std::floor(startx/tileSize);
	long startCol = std::floor(startz/tileSize);
	long goalRow = std::floor(startx/tileSize);
	long goalCol = std::floor(startz/tileSize);
	const float INF = std::numeric_limits<float>::infinity();

	tileNode start = std::make_tuple(startRow, startCol, 0);
	tileNode goal = std::make_tuple(goalRow, goalCol, INF);

	frontier.push(start);

	came_from[getTileNodeHashKey(start)] = start;
	cost_so_far[getTileNodeHashKey(start)] = 0;

	while (!frontier.empty()) {
		tileNode current = frontier.top(); // top() does not remove the node, so call pop() after it
		frontier.pop();

		if (getTileNodeHashKey(current) == getTileNodeHashKey(goal)) {
			break;
		}



		
	}
}
