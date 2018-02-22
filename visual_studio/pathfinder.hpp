#pragma once
#include <queue>
#include <unordered_map>
#include <functional>
#include <sstream>
#include "level.hpp"

namespace AI {

	/* a min heap that will store nodes we have not explored yet in the level map
	 will use it to fetch the tile node with the smallest traversal cost*/
	std::priority_queue<tileNode, std::vector<tileNode>, std::greater<int>> frontier;

	/* hash table used to check if a node was previously explored
	 so that the search algorithm does not get stuck in an infinite loop*/
	std::unordered_map<std::string, tileNode> tileNodesVisited;

	/* hash table that helps us keep track of how we reached a tileNode
	using a key of its predecessor node in the path*/
	std::unordered_map<std::string, tileNode> came_from;

	// cost associated with a path up to a certain node
	std::unordered_map<std::string, double> cost_so_far;

	/*the hash key is a string of this format row + "," + col which is unique
	for each tile*/
	std::string getTileNodeHashKey(tileNode &a);
	
	// L1 norm (manhattan distance), will be used as a hueristic for A*
	float l1_norm(tileNode &a, tileNode &b);
	
	/* find list of adjacent tile nodes which constitute possible moves
	from the position we're currently at*/
	std::vector<tileNode> getNeighbors(tileNode &a);


	void a_star(std::vector<std::vector<tileNode>> &graph, int tileSize, int startx, int startz, int goalx, int goalz);
}