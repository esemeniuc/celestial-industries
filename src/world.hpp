#pragma once

// internal
#include "common.hpp"
#include "salmon.hpp"
#include "turtle.hpp"
#include "fish.hpp"

// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World
{
public:
	World();
	~World();

	// Creates a window, sets up events and begins the game
	bool init(vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over()const;
	
	bool advanced_mode = false;

private:
	// Generates a new turtle
	bool spawn_turtle();

	// Generates a new fish
	bool spawn_fish();

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window hjandle
	GLFWwindow* m_window;

	// Game entities


	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
