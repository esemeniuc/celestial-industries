//
// Created by eric on 2/15/18.
//

#include "logger.hpp"

Logger::Logger() {
	if (Config::SYSTEM_LOG_TO_FILE) {
		std::cout << "Logging to file '" << Config::SYSTEM_LOG_FILE_PATH
				  << "' with verbosity level " << Config::SYSTEM_LOGGING_LEVEL
				  << "\n";
		this->outFile.open(Config::SYSTEM_LOG_FILE_PATH, std::ofstream::app);
	}
}
