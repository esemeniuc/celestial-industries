//
// Created by eric on 2/16/18.
//
#pragma once
#include "loglevel.hpp"

#define INF std::numeric_limits<float>::infinity()
#define EPSILON 1e-9

namespace Config {

	const int INITIAL_WINDOW_WIDTH = 1000;
	const int INITIAL_WINDOW_HEIGHT = 800;
	constexpr const char* WINDOW_TITLE = "Celestial Industries";
	const LogLevel SYSTEM_LOGGING_LEVEL = LogLevel::DEBUG;
	const bool SYSTEM_LOG_TO_FILE = true;
	constexpr const char* SYSTEM_LOG_FILE_PATH = "debug.log";
	const int CAMERA_START_POSITION_X = 20;
	const int CAMERA_START_POSITION_Y = 20;
	const int CAMERA_START_POSITION_Z = 30;
    // Note: this must be equal to the value in the shader, otherwise weird things will eventually happen probably
    const unsigned int MAX_TOTAL_SUBOBJECTS_PER_RENDERER = 1000; // If you change this and not the shader you are a bad person
}
