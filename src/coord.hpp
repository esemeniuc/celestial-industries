//
// Created by eric on 4/8/18.
//

#pragma once
#include <random>
#include <ostream>

#include "imgui.h"
#include "glm/glm.hpp" //for coord conversion
#include "common.hpp" //for hash_combine

extern std::random_device rd;     // only used once to initialise (seed) engine
extern std::mt19937 rng;    // random-number engine used (Mersenne-Twister in this case)

struct Coord {
	int colCoord, rowCoord;

	Coord() : colCoord(0), rowCoord(0) {};

	Coord(int _colCoord, int _rowCoord) : colCoord(_colCoord), rowCoord(_rowCoord) {}

	Coord(const glm::vec3& input) : colCoord(int(input.x + 0.5)), rowCoord(int(input.z + 0.5)) {}
	Coord(const ImVec2& input) : colCoord(int(input.x + 0.5)), rowCoord(int(input.y + 0.5)) {}

	bool operator==(const Coord& rhs) const {
		return rowCoord == rhs.rowCoord &&
			   colCoord == rhs.colCoord;
	}

	friend std::ostream& operator<<(std::ostream& os, const Coord& coord) {
		os << "colCoord: " << coord.colCoord << " rowCoord: " << coord.rowCoord;
		return os;
	}

	static Coord getInvalidCoord() {
		return {INT32_MIN, INT32_MIN};
	}

	bool isInvalid() {
		return *this == getInvalidCoord();
	}

	static int l1Norm(const Coord& a, const Coord& b )	{
		return abs(b.colCoord - a.colCoord) + abs(b.rowCoord - a.rowCoord);
	}

	static Coord getRandomCoord(int levelWidth, int levelHeight) {
		std::uniform_int_distribution<int> xRng(0, levelWidth - 1); // guaranteed unbiased
		std::uniform_int_distribution<int> zRng(0, levelHeight - 1); // guaranteed unbiased
		return {xRng(rng), zRng(rng)};
	}
};

struct CoordHasher {
	std::size_t operator()(const Coord& in) const noexcept {
		size_t result = std::hash<int>{}(in.rowCoord);
		hash_combine(result, std::hash<int>{}(in.colCoord));
		return result;
	}
};
