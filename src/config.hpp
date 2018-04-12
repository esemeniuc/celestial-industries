//
// Created by eric on 2/16/18.
//
#pragma once
#include "loglevel.hpp"

#define INF std::numeric_limits<float>::infinity()
#define EPSILON 1e-9

#ifdef _MSC_VER
#define font_path(name) "../data/fonts/" name
#else
#define font_path(name) "data/fonts/" name
#endif

namespace Config {
	//window setup
	const int INITIAL_WINDOW_WIDTH = 1000;
	const int INITIAL_WINDOW_HEIGHT = 800;
	constexpr const char* WINDOW_TITLE = "Celestial Industries";

	//logging
	const LogLevel SYSTEM_LOGGING_LEVEL = LogLevel::DEBUG;
	const bool SYSTEM_LOG_TO_FILE = true;
	constexpr const char* SYSTEM_LOG_FILE_PATH = "debug.log";

	//camera
	const int CAMERA_START_POSITION_X = 20;
	const int CAMERA_START_POSITION_Y = 20;
	const int CAMERA_START_POSITION_Z = 30;

	// Note: this must be equal to the value in the shader, otherwise weird things will eventually happen probably
    const unsigned int MAX_TOTAL_SUBOBJECTS_PER_RENDERER = 1000; // If you change this and not the shader you are a bad person

	//game tunable constants
	constexpr const int OBSTACLE_COST = 1000;
	constexpr const int DEFAULT_TRAVERSABLE_COST = 10;
	constexpr const int POINT_CLICK_DISTANCE_THRESHOLD = 10;
	constexpr const int RIGHT_CLICK_ATTACK_WITHIN_RANGE_THRESHOLD = 10;

	//file paths
	constexpr const char* FONTAWESOME_FILE_PATH = font_path("fa-solid-900.ttf");
	constexpr const char* KENNEY_PIXEL_FONT_FILE_PATH = font_path("KenneyPixel.ttf");
	constexpr const char* KENNEY_FUTURE_FONT_FILE_PATH = font_path("KenneyFuture.ttf");
}
