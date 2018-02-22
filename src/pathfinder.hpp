#pragma once
#include <queue>
#include <unordered_map>
#include <sstream>
#include "level.hpp"

namespace AI {
	class aStar {
	public:
		// used to establish comparison between two tileNodes
		// based on f-score
		struct Compare {
			// lambda expression
			bool operator()(tileNode &a, tileNode &b) {
				return std::get<3>(a) > std::get<3>(b);
			}
		};

		/* generate hash key as a string of this format row + "," + col
		which is unique	for each tile*/
		static std::string getTileNodeHashKey(tileNode &a);
				
		// L1 norm (manhattan distance), will be used as a hueristic for A*
		static float l1_norm(tileNode &a, tileNode &b);

		/* find list of adjacent tile nodes which constitute possible moves
		from the position we're currently at*/
		static std::vector<tileNode> getNeighbors(std::vector<std::vector<tileNode>> &graph, tileNode &current, tileNode &goal);

		static void a_star(std::vector<std::vector<tileNode>> &graph, int tileSize, int startx, int startz, int goalx, int goalz);
	};	
}