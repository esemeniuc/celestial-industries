//
// Created by eric on 2/15/18.
//
#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include "config.hpp"


/*	Example of usage
	logger(debug) << 8;
	logger << "Hello, " << "World!";
	std::string msg("plop");
	logger << msg << '\n'; //everything up to here will be logged as debug

	logger(error) << "error stuff" << 42;
	logger << "more secrets\n"; //everything in this line and the previous will be logged as error
 */


class Logger {

	std::ofstream outFile;
	const LogLevel systemLogLevel = Config::SYSTEM_LOGGING_LEVEL;
	LogLevel logLevelOfLine = disabled; //gets overwritten every time a custom level is requested

public:
	Logger();

	//need this in header otherwise we cant link, see https://isocpp.org/wiki/faq/templates#templates-defn-vs-decl
	template<class T>
	Logger& operator<<(const T& msg) {
		//use the ifndef so we don't output logs in "Release" mode
#ifndef NDEBUG
		if (logLevelOfLine <= systemLogLevel) { //show logs that are only within our verbosity limit
			std::cout << msg;
			outFile << msg;
		}
#endif
		return *this;
	}

	Logger& operator()(LogLevel logLevel) {
		logLevelOfLine = logLevel;
		std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		auto timeString = std::put_time(std::localtime(&t), "%F %T");
		std::cout << '[' << timeString << "]\t";
		outFile << '[' << timeString << "]\t";
		return *this;
	}
};

