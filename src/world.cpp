// Header
#include "world.hpp"

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
bool World::init(glm::vec2 screen)
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
	auto scroll_offset_redirect = [](GLFWwindow* wnd, double _0, double _1) { ((World*)glfwGetWindowUserPointer(wnd))->on_mouse_scroll(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetScrollCallback(m_window, scroll_offset_redirect);

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

	// setup skybox
	bool skyboxLoaded = loadSkybox("data/models/", "skybox.obj", "data/textures/skybox");
	if (!skyboxLoaded) {
		return false;
	}

	// WHY WASNT THIS ENABLED BEFORE?!
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	auto loadResult = loadTiles({
		"sand1.obj", "sand2.obj", "sand3.obj", "wall.obj", "brickCube.obj"//, "miningTower.obj", "photonTower.obj"
		});
	if (!std::get<0>(loadResult)) {
		return false;
	}
	tileTypes = std::get<1>(loadResult);
	
	level = intArrayToLevel({
		{ 3, 0, 0, 1 },
		{ 3, 1, 2, 0 },
		{ 3, 0, 1, 1 },
		{ 3, 4, 0, 2 },
	}, tileTypes);

	return true;
}

std::tuple<bool, std::vector<OBJ::Data>> World::loadTiles(std::vector<std::string> filenames)
{
	std::vector<OBJ::Data> objs;
	bool success = true;
	std::vector<std::string> pathParts;
	pathParts.push_back("data");
	pathParts.push_back("models");
	std::string path = pathBuilder(pathParts);	

	for (auto filename : filenames) {
		OBJ::Data obj;
		success &= OBJ::Loader::loadOBJ(path, filename, obj);
		objs.push_back(obj);
		if (!success) {
			std::cout << "FAILED TO LOAD OBJS! Specifically: " << filename << std::endl;
			return { success, objs };
		}
	}
	return { success, objs };
}

// skybox
bool World::loadSkybox (std::string path, std::string skyboxFilename, std::string texturePath) {
	bool success = true;
	OBJ::Data skyboxObj;

	success &= OBJ::Loader::loadOBJ(path, skyboxFilename, skyboxObj);
	if (!success) {
		std::cout << "Failed to load skybox" << std::endl;
		return false;
	}
	
	success &= m_skybox.init(skyboxObj);
	if (!success) {
		std::cout << "Failed to initilize skybox" << std::endl;
		return false;
	}

	// specify texture unit corresponding to texture sampler in fragment shader
	Texture skyboxTexture;
	GLuint cube_texture;
	glUniform1i(glGetUniformLocation(m_skybox.effect.program, "cube_texture"), 0);
	m_skybox.set_cube_faces(texturePath);
	skyboxTexture.generate_cube_map(m_skybox.get_cube_faces(), &cube_texture);
	return true;
}

std::vector<std::vector<Tile>> World::intArrayToLevel(std::vector<std::vector<int>> intArray, std::vector<OBJ::Data> tileTypes)
{
	// TODO: turn into map?
	std::vector<std::vector<Tile>> result;
	for (size_t i = 0; i < intArray.size(); i++) {
		std::vector<int> row = intArray[i];
		std::vector<Tile> tileRow;
		for (size_t j = 0; j < row.size(); j++) {
			int cell = row[j];
			Tile tile;
			bool success = tile.init(tileTypes[cell]);
			if (!success) {
				std::cout << "FAILED TO INITIALIZE TILE OF TYPE " << cell << std::endl;
			}
			// TODO: Standardize tile size and resize the model to be the correct size
			tile.translate({ j, 0, i });
			tileRow.push_back(tile);
		}
		result.push_back(tileRow);
	}
	return result;
}

// Releases all the associated resources
void World::destroy()
{
	Mix_CloseAudio();
	m_tile.destroy();
	m_skybox.destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
bool World::update(float elapsed_ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	glm::vec2 screen = glm::vec2((float)w, (float)h);
	camera.update(elapsed_ms);
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
	title_ss << "Celestial Industries";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = { 47.0/256.0,61.0/256.0, 84.0/256.0 };
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = camera.getProjectionMatrix(m_screen.x, m_screen.y);
	glm::mat4 view = camera.getViewMatrix();
	for (auto tileRow : level) {
		for (auto tile : tileRow) {
			tile.draw(projection*view);
		}
	}
	
	m_skybox.draw(projection * view * m_skybox.model);

	// Presenting
	glfwSwapBuffers(m_window);
}

// Should the game be over ?
bool World::is_over()const
{
	return glfwWindowShouldClose(m_window) || escapePressed;
}

void World::updateBoolFromKey(int action, int key, bool& toUpdate, std::vector<int> targetKeys)
{
	for (auto targetKey : targetKeys) {
		if (key == targetKey) {
			if (action == GLFW_PRESS) {
				toUpdate = true;
			}
			if (action == GLFW_RELEASE) {
				toUpdate = false;
			}
		}
	}
}

// On key callback
void World::on_key(GLFWwindow*, int key, int, int action, int mod)
{
	// Core controls
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		escapePressed = true;
	}

	// We do what we must because we can. Also we only can if we support C++11.
	std::vector<std::tuple<bool&, std::vector<int>>> stickyKeys = {
		// Format of this monstrosity:
		// { Var to update, { Keys that will update it } }

		// Camera controls:
		{ camera.move_forward, { GLFW_KEY_W, GLFW_KEY_UP } },
		{ camera.move_backward, { GLFW_KEY_S, GLFW_KEY_DOWN } },
		{ camera.move_right, { GLFW_KEY_D, GLFW_KEY_RIGHT } },
		{ camera.move_left, { GLFW_KEY_A, GLFW_KEY_LEFT } },
		{ camera.rotate_right, { GLFW_KEY_E } },
		{ camera.rotate_left, { GLFW_KEY_Q } },
		{ camera.z_held, { GLFW_KEY_Z } },
	};

	for (auto stickyKey : stickyKeys) {
		updateBoolFromKey(action, key, std::get<0>(stickyKey), std::get<1>(stickyKey));
	}
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos)
{
	// Handle the mouse movement here
}

void World::on_mouse_scroll(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.mouseScroll = glm::vec2(xoffset, yoffset);
}

