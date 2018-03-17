#pragma once
#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelHeight, size_t levelWidth) {
		entityMap = std::vector<std::vector<std::vector<Entity>>>(levelHeight, std::vector<std::vector<Entity>>(levelWidth));
	}


	void update(double elapsed_ms) {
		for (auto& row : entityMap) {
			for (auto& col : row) {
				for (auto& entityInACell : col) {
					entityInACell.move(elapsed_ms);
				}
			}
		}
	}

};


