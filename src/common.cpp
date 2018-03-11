#include <vector>
#include <sstream>
#include <string>
#include <iostream>

#include "common.hpp"

// Our stuff
Logger logger;

char separator()
{
	return '/';
}

std::vector<std::string> splitString(std::string input, const char separator) {
	std::vector<std::string> result;
	std::string segment;
	std::stringstream inputStream;
	inputStream << input;
	while (std::getline(inputStream, segment, separator)) {
		result.push_back(segment);
	}
	return result;
}

std::string pathBuilder(std::vector<std::string> parts) {
	std::stringstream path;

	// Because the template has the vs files in their own folder the exe expects stuff to be relative to the INSIDE of that folder :)
#ifdef _WIN32
		path << "../";
#endif
	for (auto part : parts) {
		path << part << separator();
	}
	return path.str();
}

std::string pathAppender(std::string base, std::vector<std::string> parts)
{
	std::stringstream path;
	path << base;
	bool first = true;
	for (auto part : parts) {
		if (!first) {
			path << separator();
		}
		else {
			first = false;
		}
		path << part;
	}
	return path.str();
}

void gl_flush_errors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool gl_has_errors()
{
    GLenum error = glGetError();

    if (error == GL_NO_ERROR) return false;

    while (error != GL_NO_ERROR)
    {
        const char* error_str = "";
        switch (error)
        {
        case GL_INVALID_OPERATION:
            error_str = "INVALID_OPERATION";
            break;
        case GL_INVALID_ENUM:
            error_str = "INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error_str = "INVALID_VALUE";
            break;
        case GL_OUT_OF_MEMORY:
            error_str = "OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error_str = "INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        logger(LogLevel::DEBUG) << "OpenGL: " << error_str << '\n';
        error = glGetError();
    }
    return true;
}
