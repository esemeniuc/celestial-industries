// Header
#include "unitmanager.hpp"
#include "unitcomp.hpp"
#include "logger.hpp"
#include "world.hpp"
#include "collisiondetection.hpp"
#include "particle.hpp"
#include "aimanager.hpp"
#include "unit.hpp"

// Same as static in c, local to compilation unit
namespace {
	namespace {
		void glfw_err_cb(int error, const char* desc) {
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World() {
	// Seeding rng with random device`
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World() = default;

std::shared_ptr<Particles::ParticleEmitter> fireSpawner;


// World initialization
bool World::init(glm::vec2 screen) {
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
		logger(LogLevel::ERR) << "Failed to initialize GLFW\n";
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 1);
	m_window = glfwCreateWindow((int) screen.x, (int) screen.y, Config::WINDOW_TITLE, nullptr, nullptr);
	m_screen = screen;
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// Load OpenGL function pointers
	gl3wInit();

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2, int _3) {
		((World*) glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3);
	};
	auto cursor_pos_redirect = [](GLFWwindow* wnd, double _0, double _1) {
		((World*) glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1);
	};
	auto scroll_offset_redirect = [](GLFWwindow* wnd, double _0, double _1) {
		((World*) glfwGetWindowUserPointer(wnd))->on_mouse_scroll(wnd, _0, _1);
	};
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetScrollCallback(m_window, scroll_offset_redirect);

	//-------------------------------------------------------------------------
	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		logger(LogLevel::ERR) << "Failed to initialize SDL Audio\n";
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		logger(LogLevel::ERR) << "Failed to open audio device\n";
		return false;
	}

	// setup skybox
	if (!loadSkybox("skybox.obj", "skybox")) {
		logger(LogLevel::ERR) << "Failed to open skybox\n";
		return false;
	}

	// WHY WASNT THIS ENABLED BEFORE?!
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);


	// Load shader for default meshSources
	objShader = std::make_shared<Shader>();
	if (!objShader->load_from_file(shader_path("objrenderable.vs.glsl"), shader_path("objrenderable.fs.glsl"))) {
		logger(LogLevel::ERR) << "Failed to load obj shader!" << '\n';
		return false;
	}

	particleShader = std::make_shared<Shader>();
    if (!particleShader->load_from_file(shader_path("particles.vs.glsl"), shader_path("particles.fs.glsl"))) {
        logger(LogLevel::ERR) << "Failed to load particle shader!" << '\n';
        return false;
    }


	if(!initMeshTypes(Model::meshSources)) {
		logger(LogLevel::ERR) << "Failed to initialize renderers \n";
	}

	levelArray = level.levelLoader(
			pathBuilder({"data", "levels"}) + "GameLevel1.txt");

	camera.position = {Config::CAMERA_START_POSITION_X, Config::CAMERA_START_POSITION_Y,
					   Config::CAMERA_START_POSITION_Z};

	level.init(levelArray, Model::meshRenderers);
	UnitManager::init(levelArray.size(), levelArray.front().size());
	// test different starting points for the AI
	aiCostMap = level.getLevelTraversalCostMap();

	//display a path
	int startx = 25, startz = 11;
	int targetx = 10, targetz = 10;
	auto temp1 = Unit::spawn(Unit::UnitType::SPHERICAL_DEATH, {startx, 0, startz}, GamePieceOwner::PLAYER);
	temp1->moveTo(targetx, targetz);

	startx = 39, startz = 19;
	auto temp2 = Unit::spawn(Unit::UnitType::SPHERICAL_DEATH, {startx, 0, startz}, GamePieceOwner::PLAYER);
	temp2->moveTo(targetx, targetz);

	startx = 39, startz = 1;
	auto temp3 = Unit::spawn(Unit::UnitType::SPHERICAL_DEATH, {startx, 0, startz}, GamePieceOwner::PLAYER);
	temp3->moveTo(targetx, targetz);

	selectedTileCoordinates.rowCoord = level.getLevelSize().rowCoord / 2;
	selectedTileCoordinates.colCoord = level.getLevelSize().colCoord / 2;
	selectedTile = level.tiles[selectedTileCoordinates.rowCoord][selectedTileCoordinates.colCoord];

	return true;
}

bool World::initMeshTypes(std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>> sources) {
	// All the models come from the same place
	std::string path = pathBuilder({"data", "models"});
	for (auto source : sources) {
		Model::MeshType tileType = source.first;
		std::vector<SubObjectSource> objSources = source.second;
		Model::meshRenderers[tileType] = std::make_shared<Renderer>(objShader, objSources);
	}
	return true;
}

// skybox
bool World::loadSkybox(const std::string& skyboxFilename, const std::string& skyboxTextureFolder) {
	OBJ::Data skyboxObj;

	std::string geometryPath = pathBuilder({"data", "models"});
	std::string texturePath = pathBuilder({"data", "textures", skyboxTextureFolder});

	if (!OBJ::Loader::loadOBJ(geometryPath, skyboxFilename, skyboxObj)) {
		logger(LogLevel::ERR) << "Failed to load skybox" << '\n';
		return false;
	}

	if (!m_skybox.init(skyboxObj)) {
		logger(LogLevel::ERR) << "Failed to initialize skybox" << '\n';
		return false;
	}

	// specify texture unit corresponding to texture sampler in fragment shader
	Texture skyboxTexture;
	GLuint cube_texture;
	glUniform1i(glGetUniformLocation(m_skybox.shader->program, "cube_texture"), 0);
	m_skybox.set_cube_faces(texturePath);
	skyboxTexture.generate_cube_map(m_skybox.get_cube_faces(), &cube_texture);
	return true;
}

// Releases all the associated resources
void World::destroy() {
	Mix_CloseAudio();
	m_skybox.destroy();
	glfwDestroyWindow(m_window);
}

// Update our game world
float total_time = 0.0f;

bool World::update(double elapsed_ms) {
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	camera.update(elapsed_ms);
	total_time += elapsed_ms;
	selectedTile->shouldDraw(true);

	if (
			selectedTileCoordinates.rowCoord >= 0 &&
			(unsigned long) selectedTileCoordinates.rowCoord < level.getLevelTraversalCostMap().size() &&
			selectedTileCoordinates.colCoord >= 0 &&
			(unsigned long) selectedTileCoordinates.colCoord < level.getLevelTraversalCostMap()[0].size()
			) {

		selectedTile = level.tiles[selectedTileCoordinates.rowCoord][selectedTileCoordinates.colCoord];
		selectedTile->shouldDraw(false);
	}

	for (const auto& turret : level.guntowers) {
		turret->update(elapsed_ms);
	}

    for (const auto& emitter : level.emitters) {
        emitter->update(elapsed_ms);
    }

	Particles::updateParticleStates(elapsed_ms);
	AiManager::update(elapsed_ms);
	UnitManager::update(elapsed_ms);

	return true;
}

// Render our game world
void World::draw() {
	// Clearing error buffer
	gl_flush_errors();

	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	m_screen = {(float) w, (float) h}; // ITS CONVENIENT TO HAVE IN FLOAT OK

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001f, 10);
	const float clear_color[3] = {47.0f / 256.0f, 61.0f / 256.0f, 84.0f / 256.0f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = camera.getProjectionMatrix(m_screen.x, m_screen.y);
	glm::mat4 view = camera.getViewMatrix();
	glm::mat4 projectionView = projection * view;

	for (auto renderer : Model::meshRenderers) {
		renderer->render(projectionView);
	}


	// make skybox rotate by 0.001 * pi/4 radians around y axis, every frame
//	float y_rotation = 0.005 * glm::quarter_pi<float>();
//	m_skybox.setRotation(glm::vec3(0.0, y_rotation, 0.0));
//	m_skybox.applyTransformations();
	m_skybox.getCameraPosition(camera.position);
	m_skybox.draw(projection * view * m_skybox.getModelMatrix());

	for (const auto &emitter : level.emitters) {
		emitter->render(projectionView, camera.position);
	}
	// Presenting
	glfwSwapBuffers(m_window);
}

void World::move_cursor_up() {
	selectedTileCoordinates.colCoord--;
	printf("Selected tile: %d, %d\n", selectedTileCoordinates.rowCoord, selectedTileCoordinates.colCoord);
}

void World::move_cursor_down() {
	selectedTileCoordinates.colCoord++;
	printf("Selected tile: %d, %d\n", selectedTileCoordinates.rowCoord, selectedTileCoordinates.colCoord);
}

void World::move_cursor_left() {
	selectedTileCoordinates.rowCoord--;
	printf("Selected tile: %d, %d\n", selectedTileCoordinates.rowCoord, selectedTileCoordinates.colCoord);
}

void World::move_cursor_right() {
	selectedTileCoordinates.rowCoord++;
	printf("Selected tile: %d, %d\n", selectedTileCoordinates.rowCoord, selectedTileCoordinates.colCoord);
}

// Should the game be over ?
bool World::is_over() const {
	return glfwWindowShouldClose(m_window) || escapePressed;
}

void World::updateBoolFromKey(int action, int key, bool& toUpdate, std::vector<int> targetKeys) {
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
void World::on_key(GLFWwindow*, int key, int, int action, int mod) {
	// Core controls
	if (action == GLFW_RELEASE && key == GLFW_KEY_R) {
		int w, h;
		glfwGetWindowSize(m_window, &w, &h);
	}

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		escapePressed = true;
	}

	// Tile selection controls:
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_I) {
			move_cursor_up();
		}
		if (key == GLFW_KEY_K) {
			move_cursor_down();
		}
		if (key == GLFW_KEY_L) {
			move_cursor_left();
		}
		if (key == GLFW_KEY_J) {
			move_cursor_right();
		}
	}

	// We do what we must because we can. Also we only can if we support C++11.
	std::vector<std::tuple<bool&, std::vector<int>>> stickyKeys = {
			// Format of this monstrosity:
			// { Var to update, { Keys that will update it } }

			// Camera controls:
			{camera.move_forward,  {GLFW_KEY_W, GLFW_KEY_UP}},
			{camera.move_backward, {GLFW_KEY_S, GLFW_KEY_DOWN}},
			{camera.move_right,    {GLFW_KEY_D, GLFW_KEY_RIGHT}},
			{camera.move_left,     {GLFW_KEY_A, GLFW_KEY_LEFT}},
			{camera.rotate_right,  {GLFW_KEY_E}},
			{camera.rotate_left,   {GLFW_KEY_Q}},
			{camera.z_held,        {GLFW_KEY_Z}},
	};

	for (auto stickyKey : stickyKeys) {
		updateBoolFromKey(action, key, std::get<0>(stickyKey), std::get<1>(stickyKey));
	}
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
	camera.pan((int) xpos, (int) ypos);

	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);

	int computedFbX = (float) xpos / (float) windowWidth * (float) framebufferWidth;
	int computedFbY = (float) ypos / (float) windowHeight * (float) framebufferHeight;

	glm::vec2 windowCoordinates{computedFbX, computedFbY};
	glm::vec2 viewport{framebufferWidth, framebufferHeight};
	glm::vec4 clipCoordinates{windowCoordinates / viewport * 2.0f - glm::vec2{1.0f}, -1.0f, 1.0f};
	clipCoordinates[1] *= -1.0;
	glm::mat4 clipWorldMatrix{
			glm::inverse(camera.getProjectionMatrix(windowWidth, windowHeight) * camera.getViewMatrix())};
	glm::vec4 unprojectedWorldCoordinates{clipWorldMatrix * clipCoordinates};
	glm::vec3 worldCoordinates{glm::vec3{unprojectedWorldCoordinates} / unprojectedWorldCoordinates.w};

	glm::vec3 directionVector = worldCoordinates - camera.position;
	glm::vec3 planeNormalVector = {0, 1, 0};
	glm::vec3 planePoint = {0, 0, 0};

	float planeDotDirection = glm::dot(planeNormalVector, directionVector);
	float t = glm::dot(planeNormalVector, (planePoint - camera.position)) / planeDotDirection;

	if (t > 0) {
		glm::vec3 pointInWorld = camera.position + (t * directionVector);
		selectedTileCoordinates.rowCoord = (int) round(pointInWorld.z);
		selectedTileCoordinates.colCoord = (int) round(pointInWorld.x);
	}


}

void World::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.mouseScroll = glm::vec2(xoffset, yoffset);
}

