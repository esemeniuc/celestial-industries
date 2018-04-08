#pragma once

#include "config.hpp"
#include "logger.hpp"
#include "imgui.h" //for imvec2 constructor

// stdlib
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <random>

// glfw
#define NOMINMAX

#include <gl3w.h>
#include <GLFW/glfw3.h>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

//ignore warnings from glm string_cast
#pragma GCC diagnostic push // save diagnostic state
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#define GLM_ENABLE_EXPERIMENTAL //for string printing for glm vec and mat

#include "glm/gtx/string_cast.hpp"

#pragma GCC diagnostic pop

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
#ifdef _MSC_VER
#define data_path "../data"
#define shader_path(name) "../shaders/" name
#else
#define data_path "data"
#define shader_path(name) "shaders/" name
#endif
#define textures_path(name) data_path "/textures/" name
#define audio_path(name) data_path "/audio/" name
#define mesh_path(name) data_path "/meshes/" name


// Our stuff
extern Logger logger; //use extern so we share the logger in main.cpp with all other cpp files
extern std::random_device rd;     // only used once to initialise (seed) engine
extern std::mt19937 rng;    // random-number engine used (Mersenne-Twister in this case)


std::vector<std::string> splitString(std::string input, const char separator);

char separator();

std::string pathBuilder(std::vector<std::string> parts);

std::string pathAppender(std::string base, std::vector<std::string> parts);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();

bool gl_has_errors();

// Because iterating over pointer is good
template<typename T>
auto inline begin(std::shared_ptr<T> ptr) -> typename T::iterator {
	return ptr->begin();
}

template<typename T>
auto inline end(std::shared_ptr<T> ptr) -> typename T::iterator {
	return ptr->end();
}

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

//from boost to combine hashes effectively
template<typename SizeT>
inline void hash_combine(SizeT& seed, SizeT value) {
	seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

struct CoordHasher {
	std::size_t operator()(const Coord& in) const noexcept {
		size_t result = std::hash<int>{}(in.rowCoord);
		hash_combine(result, std::hash<int>{}(in.colCoord));
		return result;
	}
};

template<class T>
T clamp(T lower, T value, T upper) {
	if (value > upper) {
		value = upper;
	}

	if (value < lower) {
		value = lower;
	}
	return value;
}

long getUnixTime();

