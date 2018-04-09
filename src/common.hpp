#pragma once

#include "config.hpp"
#include "logger.hpp"

// stdlib
#include <memory>
#include <string>
#include <vector>

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

//from boost to combine hashes effectively
template<typename SizeT>
inline void hash_combine(SizeT& seed, SizeT value) {
	seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

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

