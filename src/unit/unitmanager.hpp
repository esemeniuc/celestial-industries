#pragma once
#include <vector>
#include "global.hpp"


namespace UnitManager {

	void init(size_t levelWidth, size_t levelHeight) {
		entityMap = std::vector<std::vector<std::vector<Entity>>>(levelWidth, std::vector<std::vector<Entity>>(levelHeight));
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


