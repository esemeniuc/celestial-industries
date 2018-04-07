// internal
#include "common.hpp"
#include "world.hpp"
#include "ui.hpp"

#define GL3W_IMPLEMENTATION

#include <gl3w.h>

// stlib
#include <chrono>
using Clock = std::chrono::high_resolution_clock;
void renderDrawData(ImDrawData* draw_data);

// Entry point
int main(int argc, char* argv[]) {
	logger(LogLevel::DEBUG) << "Started game\n";
	// Initializing world (after renderer.init().. sorry)
	if (!World::init()) {
		// Time to read the error message
		logger(LogLevel::ERR) << "Press any key to exit" << '\n';
		std::cin.get();
		return EXIT_FAILURE;
	}

	Ui::imguiSetup(World::m_window);

	auto t = Clock::now();
	// variable timestep loop.. can be improved (:
	while (!World::is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		double elapsed_microSec = (double) (std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count();
		double elapsed_milliSec = elapsed_microSec / 1000;
		t = now;
		//if time is too long (eg in breakpoint during debug, the clamp the elapsed time)
		if (elapsed_milliSec > 1000) {
			elapsed_milliSec = 1000 / 60.0; //pretend we continue to next frame at 60fps
		}

		World::update(elapsed_milliSec);
		World::draw();
		Ui::imguiGenerateScreenObjects();
	}

	World::destroy();

	return EXIT_SUCCESS;
}

