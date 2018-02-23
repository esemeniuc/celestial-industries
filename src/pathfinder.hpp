#pragma once

#include <algorithm>
#include <queue>
#include <unordered_map>
#include "level.hpp"

namespace AI {
	class aStar {
	public:
		// used to establish comparison between two tileNodes
		// based on f-score
		struct Compare {
			// lambda expression
			bool operator()(const aStarPathState& a, const aStarPathState& b) {
				return a.fScore > b.fScore;
			}
		};

		/* generate hash key which is fairly unique for each tile*/
		struct aStarPathHasher {
			std::size_t operator()(const aStarPathState& in) const noexcept {
				size_t result = std::hash<int>{}(in.rowCoord);
				result = 31 * result + std::hash<int>{}(in.colCoord);
				return result; // or use boost::hash_combine
			}
		};

		// L1 norm (manhattan distance), will be used as a heuristic for A*
		static float l1_norm(const aStarPathState& a, const aStarPathState& b);

		/* find list of adjacent tile nodes which constitute possible moves
		from the position we're currently at*/
		static std::vector<aStarPathState>
		getNeighbors(const std::vector<std::vector<aStarPathState>>& graph, aStarPathState& current,
					 aStarPathState& goal);

		static std::pair<bool, std::vector<aStarPathState>>
		a_star(const std::vector<std::vector<aStarPathState>>& graph, int tileSize,
			   int startx, int startz, int goalx, int goalz);

		static std::vector<aStarPathState>
		reconstruct_path(const std::unordered_map<aStarPathState, aStarPathState, aStarPathHasher>& came_from,
						 const aStarPathState& start, const aStarPathState& goal);
	};
}