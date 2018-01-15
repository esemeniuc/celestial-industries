// Header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <cmath>

// glm
#include "glm\mat4x4.hpp"
#include "glm\gtc\matrix_transform.hpp"

// glfw
#include "GLFW\glfw3.h"

// Same as static in c, local to compilation unit
namespace
{
	const size_t TILE_WIDTH = 10;

	namespace
	{
		void glfw_err_cb(int error, const char* desc)
		{
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World(){
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{

}

// World initialization
bool World::init(vec2 screen)
{
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)screen.x, (int)screen.y, "A1 Assignment", nullptr, nullptr);
	m_screen = screen;
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3w_init();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) { ((World*)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}


	// WHY WASNT THIS ENABLED BEFORE?!
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	Tile tile;
	bool tileInit = tile.init();
	m_tile = tile;
	return tileInit;
}

// Releases all the associated resources
void World::destroy()
{
	Mix_CloseAudio();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
        glfwGetFramebufferSize(m_window, &w, &h);
	vec2 screen = { (float)w, (float)h };
	cameraAngle.x += mouseSpeed * mouseMovement.x * elapsed_ms;
	cameraAngle.y += mouseSpeed * mouseMovement.y * elapsed_ms;

	cameraDirection = glm::vec3(
		cos(cameraAngle.y)*sin(cameraAngle.x),
		sin(cameraAngle.y),
		cos(cameraAngle.y)*cos(cameraAngle.x)
	);
	cameraHorizontalVector = glm::vec3(sin(cameraAngle.x - M_PI / 2.0f), 0, cos(cameraAngle.x - M_PI / 2.0f));
	cameraVerticalVector = glm::cross(cameraHorizontalVector, cameraDirection);
	if (key_up) cameraPosition += cameraDirection * elapsed_ms * cameraSpeed;
	if (key_down) cameraPosition -= cameraDirection * elapsed_ms * cameraSpeed;
	if (key_right) cameraPosition += cameraHorizontalVector * elapsed_ms * cameraSpeed;
	if (key_left) cameraPosition += cameraHorizontalVector * elapsed_ms * cameraSpeed;
	return true;
}

// Render our game world
void World::draw()
{
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
    glfwGetFramebufferSize(m_window, &w, &h);
	m_screen = { (float)w, (float)h }; // ITS CONVENIENT TO HAVE IN FLOAT OK

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "TEST";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 47.0/256.0,61.0/256.0, 84.0/256.0 };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = glm::perspective(glm::radians(fieldOfView), m_screen.x / m_screen.y, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraVerticalVector);
	glm::mat4 model = glm::mat4(1.0f); // Identity matrix | model is at origin and not scaled or rotated or anything else
	m_tile.draw(projection*view*model);

	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
	return glfwWindowShouldClose(m_window) || escapePressed;
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
		key_up = true;
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_UP) {
		key_up = false;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
		key_down = true;
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_DOWN) {
		key_down = false;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT) {
		key_right = true;
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_RIGHT) {
		key_right = false;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_LEFT) {
		key_left = true;
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_LEFT) {
		key_left = false;
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		escapePressed = true;
	}
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// Stop the mouse from escaping
	glfwSetCursorPos(window, m_screen.x / 2, m_screen.y / 2);
	mouseMovement.x = m_screen.x/2 - xpos;
	mouseMovement.y = m_screen.y/2 - ypos;
}

