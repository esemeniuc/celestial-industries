//
// Created by eric on 2/15/18.
//
#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>

#include "config.hpp"
#include "loglevel.hpp"


/*	Example of usage
	logger(LogLevel::DEBUG) << 8;
	logger << "Hello, " << "World!";
	std::string msg("plop");
	logger << msg << '\n'; //everything up to here will be logged as debug

	logger(LogLevel::ERR) << "error stuff" << 42;
	logger << "more secrets\n"; //everything in this line and the previous will be logged as error
 */


class Logger {

	std::ofstream outFile;
	const LogLevel systemLogLevel = Config::SYSTEM_LOGGING_LEVEL;
	LogLevel logLevelOfLine = LogLevel::DISABLED; //this variable gets overwritten every time a custom level is requested through operator()

public:
	Logger();

    static constexpr char endl = '\n';

	Logger& operator()(LogLevel logLevel);

	//need this function in header otherwise compiler can't link, see https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
	template<class T>
	Logger& operator<<(const T& msg) {
		//use the ifndef so we don't output logs in "Release" mode
#ifndef NDEBUG
		if (logLevelOfLine <= systemLogLevel) { //show logs that are only within our verbosity limit
			std::cout << msg;
			if (Config::SYSTEM_LOG_TO_FILE) {
				outFile << msg;
			}
		}
#endif
		return *this;
	}
};

