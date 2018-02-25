#pragma once

// internal
#include "common.hpp"
#include "tile.hpp"
#include "skybox.hpp"
#include "camera.hpp"
#include "level.hpp"
#include "pathfinder.hpp"

// stlib
#include <vector>
#include <random>
#include <cstring>
#include <cassert>
#include <sstream>
#include <cmath>
#include <map>
#include <iostream>

#define SDL_MAIN_HANDLED

#include <SDL.h>
#include <SDL_mixer.h>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

// glfw
#include "GLFW/glfw3.h"


// Container for all our entities and game logic. Individual rendering / update is 
// deferred to the relative update() methods
class World {
public:
	//members
	bool advanced_mode = false;

	//funcs
	World();

	~World();

	// Creates a window, sets up events and begins the game
	bool init(glm::vec2 screen);

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(float ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over() const;

private:
	//members
	// Window handle
	GLFWwindow* m_window;
	bool escapePressed = false;
	glm::vec2 m_screen;

	// Camera stuff
	Camera camera;

	// Selection
	std::vector<int> selectedTileCoordinates;
    std::shared_ptr<Tile> selectedTile;

	// Game entities
	std::shared_ptr<Shader> objShader;
	Level level;
	Skybox m_skybox;

	// C++ rng
	std::default_random_engine m_rng;
	std::uniform_real_distribution<float> m_dist; // default 0..1

	//funcs
	void move_cursor_up();

	void move_cursor_down();

	void move_cursor_left();

	void move_cursor_right();

	void updateBoolFromKey(int action, int key, bool& toUpdate, std::vector<int> targetKeys);

	bool loadSkybox(const std::string& skyboxFilename, const std::string& skyboxTextureFolder);

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow*, int key, int, int action, int mod);

	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

};
