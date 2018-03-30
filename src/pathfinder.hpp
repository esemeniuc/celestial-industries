#pragma once

#include <algorithm>
#include <queue>
#include <unordered_map>
#include "level.hpp"

namespace AI {
	namespace aStar {
		bool isTraversable(int x, int z);

		// used to establish comparison between two tileNodes
		// based on f-score
		struct aStarComparator {
			// lambda expression
			bool operator()(const AStarNode& a, const AStarNode& b) {
				return a.fScore > b.fScore;
			}
		};

		/* generate hash key which is fairly unique for each tile*/
		struct aStarHasher {
			std::size_t operator()(const AStarNode& in) const noexcept {
				size_t result = std::hash<int>{}(in.rowCoord);
				hash_combine(result, std::hash<int>{}(in.colCoord));
				return result;
			}
		};

		// L1 norm (manhattan distance), will be used as a heuristic for A*
		float l1_norm(const AStarNode& a, const AStarNode& b);

		float l2_norm(const AStarNode& startNode, const AStarNode& goal);

		// find list of adjacent tile nodes which constitute possible moves from the position we're currently at
		std::vector<AStarNode> getNeighbors(const std::vector<std::vector<AStarNode>>& graph, AStarNode& currentPos,
											AStarNode& goal);

		//main pathfinding algorithm
		std::pair<bool, std::vector<Coord>> findPath(int tileSize, int startX, int startZ, int goalX, int goalZ);

		std::vector<Coord> reconstruct_path(const std::unordered_map<AStarNode, AStarNode, aStarHasher>& came_from,
											const AStarNode& start, const AStarNode& goal);
	}
}
