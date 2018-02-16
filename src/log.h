//
// Created by eric on 2/15/18.
//

#pragma once

#include <iostream>
#include <sstream>
#include "config.hpp"
class Log {
	enum level {
		disabled, error, warn, info, debug //higher numbers for more verbose
	};

	const level systemLogLevel = SYSTEM_LOGGING_LEVEL;
	level logLevelOfLine = disabled;


public:
	Log();

	template<class T>
	Log& operator<<(const T& msg) {

		//use the ifndef so we don't output logs in "Release" mode
		#ifndef NDEBUG
		if(logLevelOfLine <= systemLogLevel){ //show logs that are only within our verbosity limit
			std::cout << msg;
		}
		#endif

		return *this;
	}

	Log& operator()(Log::level logLevel) {
		logLevelOfLine = logLevel;
		return *this;
	}

};

