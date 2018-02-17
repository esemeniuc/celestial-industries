//
// Created by eric on 2/15/18.
//
#pragma once

#include <iostream>
#include <sstream>
#include "config.hpp"

class Logger {

	const LogLevel systemLogLevel = Config::SYSTEM_LOGGING_LEVEL;
	LogLevel logLevelOfLine = disabled; //gets overwritten every time a custom level is requested

public:
	Logger();

	template<class T>
	Logger& operator<<(const T& msg) {
		//use the ifndef so we don't output logs in "Release" mode
#ifndef NDEBUG
		if (logLevelOfLine <= systemLogLevel) { //show logs that are only within our verbosity limit
			std::cout << msg;
		}
#endif
		return *this;
	}

	Logger& operator()(LogLevel logLevel) {
		logLevelOfLine = logLevel;
		return *this;
	}

};

