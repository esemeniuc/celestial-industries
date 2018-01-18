#pragma once

// internal
#include "common.hpp"
#include "tile.hpp"
// stlib
#include <vector>
#include <random>

#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

// glm
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"

struct TimeTile {
	OBJData present;
	OBJData past;
};

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

	std::vector<OBJData> tileTypes;

	std::vector<std::vector<TimeTile>> level;

private:
	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);
	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

private:
	// Window handle
	GLFWwindow* m_window;
	bool escapePressed = false;

	Tile m_tile;
	vec2 m_screen;
	glm::mat4 m_view;

	// Camera stuff
	float angleTowardsZ = M_PI;
	float fieldOfView = 50.0f;
	float cameraSpeed = 0.1f;
	float mouseSpeed = 0.00005f;
	vec2 mouseMovement = { 0, 0 };
	vec2 cameraAngle = { 0,0 }; // horizontal (along x), vertical (along y).
	glm::vec3 cameraDirection;
	glm::vec3 cameraPosition;
	glm::vec3 cameraVerticalVector; // Note: This is what is typically reffered to as "up" for the viewer
	glm::vec3 cameraHorizontalVector;


	bool key_up, key_down, key_right, key_left;
	
	// Game entities

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1
};
