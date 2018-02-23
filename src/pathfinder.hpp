#pragma once
#include <queue>
#include <unordered_map>
#include <sstream>
#include <algorithm> 
#include "level.hpp"

namespace AI {
	class aStar {
	public:
		// used to establish comparison between two tileNodes
		// based on f-score
		struct Compare {
			// lambda expression
			bool operator()(const aStarPathState &a, const aStarPathState &b) {
				return a.fScore > b.fScore;
			}
		};

		/* generate hash key as a string of this format row + "," + col
		which is unique	for each tile*/
		static std::string getTileNodeHashKey(const aStarPathState &a);
				
		// L1 norm (manhattan distance), will be used as a heuristic for A*
		static float l1_norm(aStarPathState &a, aStarPathState &b);

		/* find list of adjacent tile nodes which constitute possible moves
		from the position we're currently at*/
		static std::vector<aStarPathState> getNeighbors(std::vector<std::vector<aStarPathState>> &graph, aStarPathState &current, aStarPathState &goal);

		static std::pair<bool, std::vector<aStarPathState>> a_star(std::vector<std::vector<aStarPathState>> graph, int tileSize,
															 int startx, int startz, int goalx, int goalz);

		static std::vector<aStarPathState> reconstruct_path(std::unordered_map<std::string, aStarPathState> &came_from, const aStarPathState &start, const aStarPathState &goal);
	};	
}