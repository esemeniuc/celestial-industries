//
// Created by eric on 2/16/18.
//
#pragma once

#include "loglevel.hpp"

namespace Config {

	const LogLevel SYSTEM_LOGGING_LEVEL = LogLevel::DEBUG;
	const bool SYSTEM_LOG_TO_FILE = true;
	constexpr const char* SYSTEM_LOG_FILE_PATH = "debug.log";
};


