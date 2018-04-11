// internal
#include "common.hpp"
#include "world.hpp"
#include "ui.hpp"
#include "global.hpp" //for gamestate

#define GL3W_IMPLEMENTATION

#include <gl3w.h>
#include <chrono>

using Clock = std::chrono::high_resolution_clock;

void gameLoop() {
	auto t = Clock::now();
	// variable timestep loop.. can be improved (:
	while (!World::is_over() && Global::gameState == GameState::PLAY) {
		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		double elapsed_milliSec = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
				now - t).count());
		t = now;
		//if time is too long (eg in breakpoint during debug, then clamp the elapsed time)
		if (elapsed_milliSec > 1000) {
			elapsed_milliSec = 1000 / 60.0; //pretend we continue to next frame at 60fps
		}

		World::update(elapsed_milliSec);
		World::draw();
		Ui::imguiGenerateScreenObjects();
	}
}


// Entry point
int main(int argc, char* argv[]) {
	logger(LogLevel::DEBUG) << "Started game\n";

	if (!Ui::createWindow()) {
		logger(LogLevel::ERR) << "Window creation failed\n";
		return EXIT_FAILURE;
	}

	Ui::imguiSetup();
	World::m_window = Ui::getWindow();

	// Initializing world (after renderer.init().. sorry)
	if (!World::init()) {
		// Time to read the error message
		logger(LogLevel::ERR) << "Press any key to exit\n";
		std::cin.get();
		return EXIT_FAILURE;
	}

	while (Global::gameState != GameState::QUIT) {

		//note that the functions will change Global::gameState
		if (glfwWindowShouldClose(Ui::getWindow())) {
			Global::gameState = GameState::QUIT;
		}
		switch (Global::gameState) {
			case GameState::START_MENU: {
				printf("menu\n");
				Ui::imguiDrawLaunchMenu(); //once this finishes we draw the world
				break;
			}
			case GameState::PLAY: {
				printf("play\n");
				gameLoop();
				break;
			}
			case GameState::PAUSED: {
				//draw game pause menu
				Ui::imguiDrawPauseMenu(); //once this finishes we draw the world
				break;
			}
			case GameState::QUIT: {
				World::destroy();
				Ui::imguiShutdown();
				return EXIT_SUCCESS;
			}
		}
	}
}