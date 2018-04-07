#pragma once

// internal
#include "camera.hpp"
#include "common.hpp"
#include "level.hpp"
#include "skybox.hpp"
#include "tile.hpp"
#include "model.hpp"

// stdlib
#include <memory> //for shared_ptr
#include <random>
#include <vector>

//sdl stuff
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
namespace World {
	//members
	// Window handle
	extern GLFWwindow* m_window;

	extern bool escapePressed;
	extern glm::vec2 m_screen;

	// Camera stuff
	extern Camera camera;

	// Selection
	extern Coord selectedTileCoordinates;

	// Game entities
	extern std::shared_ptr<Shader> objShader;
	extern Level level;
	extern Skybox m_skybox;

	// Particle things
	extern std::shared_ptr<Shader> particleShader;

	// C++ rng
	extern std::default_random_engine m_rng;
	extern std::uniform_real_distribution<float> m_dist; // default 0..1

	extern double total_time;

	//funcs
	// Creates a window, sets up events and begins the game
	bool init();

	// Releases all associated resources
	void destroy();

	// Steps the game ahead by ms milliseconds
	bool update(double ms);

	// Renders our scene
	void draw();

	// Should the game be over ?
	bool is_over();

	//funcs
	void move_cursor_up();

	void move_cursor_down();

	void move_cursor_left();

	void move_cursor_right();

	void updateBoolFromKey(int action, int key, bool& toUpdate, std::vector<int> targetKeys);

	bool loadSkybox(const std::string& skyboxFilename, const std::string& skyboxTextureFolder);

	bool initMeshTypes(std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> sources);

	// !!! INPUT CALLBACK FUNCTIONS
	void on_key(GLFWwindow* window, int key, int scancode, int action, int mods);

	void on_mouse_move(GLFWwindow* window, double xpos, double ypos);

	void on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset);

	void on_mouse_button(GLFWwindow* window, int button, int action, int mods);

	void on_window_resize(GLFWwindow* window, int width, int height);
}
