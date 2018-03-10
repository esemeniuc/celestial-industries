#pragma once

// internal
#include "camera.hpp"
#include "common.hpp"
#include "level.hpp"
#include "pathfinder.hpp"
#include "skybox.hpp"
#include "tile.hpp"

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
class World {
public:
	//members
	bool advanced_mode = false;
    std::map<Model::MeshType, std::shared_ptr<Renderer>> meshRenderers;
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
	Coord selectedTileCoordinates;
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

    bool initMeshTypes(std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> sources);
};
