// internal
#include "common.hpp"
#include "world.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// Global 
World world;
Logger logger;

// Entry point
int main(int argc, char* argv[])
{
//
//	for (int i = 0; i <= 10; ++i) {
//		glm::dvec3 d = glm::mix(glm::vec3(5.0f, 6.0f, 0.0f),glm::vec3(8.0f,10.0f,0.0f),(float)i/10);
//
////		std::cout << d.x <<' ' << d.y <<' '<< d.z << '\n';
//	}

	logger(LogLevel::DEBUG) << "Started game\n";
	// Initializing world (after renderer.init().. sorry)
	if (!world.init({ (float)Config::WINDOW_WIDTH, (float)Config::WINDOW_HEIGHT }))
	{
		// Time to read the error message
        logger(LogLevel::ERR) << "Press any key to exit" << '\n';
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();

	// variable timestep loop.. can be improved (:
	while (!world.is_over())
	{
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_sec = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		world.update(elapsed_sec);
		world.draw();
	}

	world.destroy();

	return EXIT_SUCCESS;
}