//
// Created by eric on 2/16/18.
//
#pragma once

enum LogLevel {
	disabled, error, warn, info, debug //higher numbers for more verbosity
};

namespace Config {

	const LogLevel SYSTEM_LOGGING_LEVEL = debug;
	const bool SYSTEM_LOG_TO_FILE = true;
	constexpr const char* SYSTEM_LOG_FILE_PATH = "debug.log";
};


