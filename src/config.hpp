//
// Created by eric on 2/16/18.
//
#pragma once

#include "loglevel.hpp"

namespace Config {

	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	constexpr const char* WINDOW_TITLE = "Celestial Industries";
	const LogLevel SYSTEM_LOGGING_LEVEL = LogLevel::DEBUG;
	const bool SYSTEM_LOG_TO_FILE = true;
	constexpr const char* SYSTEM_LOG_FILE_PATH = "debug.log";
	const int CAMERA_START_POSITION_X = 20;
	const int CAMERA_START_POSITION_Y = 20;
//	const int CAMERA_START_POSITION_Z = 30;
	const int CAMERA_START_POSITION_Z = -15;

	enum class MeshType {
		SAND_1,
		SAND_2,
		SAND_3,
		WALL,
		BRICK_CUBE,
		MINING_TOWER,
		PHOTON_TOWER,
		TREE,
		GUN_TURRET,
		BALL
	};
}
