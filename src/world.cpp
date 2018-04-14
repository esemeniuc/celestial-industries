// Header
#include "unitmanager.hpp"
#include "logger.hpp"
#include "world.hpp"
//#include "collisiondetection.hpp"
#include "particle.hpp"
#include "aimanager.hpp"
#include "unit.hpp"
#include "attackManger.hpp"
#include "buildingmanager.hpp"

namespace World {
	GLFWwindow* m_window;

	glm::vec2 m_screen;

	// Camera stuff
	Camera camera;

	// Selection
	Coord selectedTileCoordinates;

	// Game entities
	std::shared_ptr<Shader> objShader;
	Level level;
	Skybox m_skybox;

	// Particle things
	std::shared_ptr<Shader> particleShader;

	// C++ rng
	std::default_random_engine m_rng = std::default_random_engine(std::random_device()());
	std::uniform_real_distribution<float> m_dist; // default 0..1

	// music and audio
	Mix_Music* m_background_music;
	Mix_Chunk* m_mouse_click;

	double gameElapsedTime = 0.0;
}


// World initialization
bool World::init() {
	m_screen = glm::vec2(Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOW_HEIGHT);
	//the 4 below are done in ui.cpp now
//	// Setting callbacks to member functions (that's why the redirect is needed)
//	// Input is handled using GLFW, for more info see
//	// http://www.glfw.org/docs/latest/input_guide.html
//	glfwSetKeyCallback(m_window, on_key);
//	glfwSetCursorPosCallback(m_window, on_mouse_move);
//	glfwSetScrollCallback(m_window, on_mouse_scroll);
//	glfwSetMouseButtonCallback(m_window, on_mouse_button);
	glfwSetWindowSizeCallback(m_window, on_window_resize);

	//-------------------------------------------------------------------------

	// Loading music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		logger(LogLevel::ERR) << "Failed to initialize SDL Audio\n";
		return false;
	}

	// load support for the OGG audio format
	int flags = MIX_INIT_OGG;
	int initted = Mix_Init(flags);
	if ((initted & flags) != flags) {
		logger(LogLevel::ERR) << "Mix_Init: Failed to init required ogg support!\n";
		logger(LogLevel::ERR) << "Mix_Init: " << Mix_GetError() << '\n';
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		logger(LogLevel::ERR) << "Failed to open audio device";
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("game_sound_track.ogg"));
	// LoadWAV is actaully capable of loading other audio formats as well, the name is not accurate
	// https://www.libsdl.org/projects/SDL_mixer/docs/SDL_mixer_19.html#SEC19
	m_mouse_click = Mix_LoadWAV(audio_path("click3.ogg"));

	if (m_background_music == nullptr) {
		logger(LogLevel::ERR) << "Failed to load sounds, make sure the data directory is present";
		return false;
	}

	// Playing background music indefinitely
	Mix_PlayMusic(m_background_music, -1);
	logger(LogLevel::DEBUG) << "Loaded music";

	// setup skybox
	if (!loadSkybox("skybox.obj", "skybox")) {
		logger(LogLevel::ERR) << "Failed to open skybox\n";
		return false;
	}

	// Load shader for default meshSources
	objShader = std::make_shared<Shader>();
	if (!objShader->load_from_file(shader_path("celShader.vs.glsl"), shader_path("celShader.fs.glsl"))) {
		logger(LogLevel::ERR) << "Failed to load obj shader!" << '\n';
		return false;
	}

	particleShader = std::make_shared<Shader>();
	if (!particleShader->load_from_file(shader_path("particles.vs.glsl"), shader_path("particles.fs.glsl"))) {
		logger(LogLevel::ERR) << "Failed to load particle shader!" << '\n';
		return false;
	}

	if (!initMeshTypes(Model::meshSources)) {
		logger(LogLevel::ERR) << "Failed to initialize renderers\n";
	}

	int windowWidth, windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	Global::windowWidth = static_cast<size_t>(windowWidth);
	Global::windowHeight = static_cast<size_t>(windowHeight);

	camera.position = {Config::CAMERA_START_POSITION_X, Config::CAMERA_START_POSITION_Y,
					   Config::CAMERA_START_POSITION_Z};

	Global::levelArray = level.levelLoader(pathBuilder({"data", "levels"}) + "GameLevel1.txt", particleShader);
	Global::levelHeight = Global::levelArray.size();
	Global::levelWidth = Global::levelArray.front().size();
	level.init(Model::meshRenderers);

	UnitManager::init(Global::levelHeight, Global::levelWidth);
	AI::Manager::init(Global::levelHeight, Global::levelWidth);
	Global::aStarCostMap = level.getLevelTraversalCostMap();

	//display a path
	int startx = 25, startz = 11;
	int targetx = 10, targetz = 10;
	auto temp1 = Unit::spawn(Model::MeshType::BALL, {startx, 0, startz}, GamePieceOwner::PLAYER);
//	temp1->moveTo(UnitState::MOVE, targetx, targetz);

	startx = 39, startz = 19;
	auto temp2 = Unit::spawn(Model::MeshType::FRIENDLY_RANGED_UNIT, {startx, 0, startz}, GamePieceOwner::PLAYER);
//	temp2->moveTo(targetx, targetz);

	startx = 39, startz = 1;
	auto temp3 = Unit::spawn(Model::MeshType::BALL, {startx, 0, startz}, GamePieceOwner::PLAYER);
//	temp3->moveTo(UnitState::MOVE, targetx, targetz);

	startx = 20, startz = 20;
	auto temp4 = Unit::spawn(Model::MeshType::FRIENDLY_RANGED_UNIT, {startx, 0, startz}, GamePieceOwner::PLAYER);

	// Example use of targeting units.
//	AttackManager::registerTargetUnit(temp2, temp1);

	//don't set selectedTileCoords at launch because glfwGetCursorPos() returns weird stuff
	return true;
}

bool World::initMeshTypes(const std::vector<std::pair<Model::MeshType, std::vector<SubObjectSource>>>& sources) {
	// All the models come from the same place
	std::string path = pathBuilder({"data", "models"});
	for (const auto& source : sources) {
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
	if (m_background_music != nullptr) {
		Mix_FreeMusic(m_background_music);
	}

	if (m_mouse_click != nullptr) {
		Mix_FreeChunk(m_mouse_click);
	}

	// cleans up all dynamically loaded library handles used by sdl mixer
	Mix_CloseAudio();
	Mix_Quit();

	m_skybox.destroy();
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

// Update our game world
bool World::update(double elapsed_ms) {
	glfwPollEvents(); //Processes system messages, if this wasn't present the window would become unresponsive
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);
	camera.update(elapsed_ms);
	gameElapsedTime += elapsed_ms;

	if (selectedTileCoordinates.rowCoord >= 0 &&
		(unsigned long) selectedTileCoordinates.rowCoord < level.getLevelTraversalCostMap().size() &&
		selectedTileCoordinates.colCoord >= 0 &&
		(unsigned long) selectedTileCoordinates.colCoord < level.getLevelTraversalCostMap()[0].size()) {

		level.tileCursor->setPosition({selectedTileCoordinates.colCoord, 0, selectedTileCoordinates.rowCoord});
	}

	for (const auto& emitter : level.emitters) {
		emitter->update(elapsed_ms);
	}

	AI::Manager::update(elapsed_ms);
	UnitManager::update(elapsed_ms);
	AttackManager::update(elapsed_ms);
	BuildingManager::update(elapsed_ms);

	Model::collisionDetector.findCollisions(elapsed_ms);
	for (const auto& tile : level.tiles) {
		tile->update(elapsed_ms);
	}
	for (const auto& entity : Global::playerUnits) {
		entity->animate(elapsed_ms);
	}
//	if (m_dist(m_rng) < 0.005) {
//		int row = m_dist(m_rng) * Global::levelWidth;
//		int col = m_dist(m_rng) * Global::levelHeight;
//		if (level.getLevelTraversalCostMap()[col][row].movementCost < 50.0f) {
//			glm::vec3 pos = glm::vec3(row, 0, col);
//			float unitRand = m_dist(m_rng);
//			if (unitRand < 0.33) {
//				level.placeEntity(Model::MeshType::ENEMY_SPIKE_UNIT, pos, GamePieceOwner::AI);
//			} else if (unitRand < 0.66) {
//				level.placeEntity(Model::MeshType::ENEMY_RANGED_LINE_UNIT, pos, GamePieceOwner::AI);
//			} else {
//				level.placeEntity(Model::MeshType::ENEMY_RANGED_RADIUS_UNIT, pos, GamePieceOwner::AI);
//			}
//		}
//	}
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

	for (const auto& renderer : Model::meshRenderers) {
		renderer->render(projectionView, view);
	}

	m_skybox.getCameraPosition(camera.position);
	m_skybox.draw(projection * view * m_skybox.getModelMatrix());

	for (const auto& emitter : level.emitters) {
		emitter->render(projectionView, camera.position);
	}
	// Presenting
//	glfwSwapBuffers(m_window);
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

void World::play_mouse_click_sound() {
	Mix_PlayChannel(-1, m_mouse_click, 0);
}

// Should the game be over ?
bool World::is_over() {
	return glfwWindowShouldClose(m_window);
}

void World::updateBoolFromKey(int action, int key, bool& toUpdate, const std::vector<int>& targetKeys) {
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
void World::on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Core controls
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		Global::gameState = GameState::QUIT;
	} else if (action == GLFW_PRESS && key == GLFW_KEY_P) {
		Global::gameState = GameState::PAUSED;
	}

	// File saving
	if (action == GLFW_RELEASE && key == GLFW_KEY_O) {
		level.save("savedLevel.txt");
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


std::pair<bool, glm::vec3> World::getTileCoordFromWindowCoords(double xpos, double ypos) {
	int framebufferWidth, framebufferHeight;
	glfwGetFramebufferSize(m_window, &framebufferWidth, &framebufferHeight);

	double computedFbX = (xpos / Global::windowWidth) * framebufferWidth;
	double computedFbY = (ypos / Global::windowHeight) * framebufferHeight;

	glm::vec2 windowCoordinates{computedFbX, computedFbY};
	glm::vec2 viewport{framebufferWidth, framebufferHeight};
	glm::vec4 clipCoordinates{windowCoordinates / viewport * 2.0f - glm::vec2{1.0f}, -1.0f, 1.0f};
	clipCoordinates[1] *= -1.0;
	glm::mat4 clipWorldMatrix{glm::inverse(camera.getProjectionMatrix(Global::windowWidth,
																	  Global::windowHeight) * camera.getViewMatrix())};
	glm::vec4 unprojectedWorldCoordinates{clipWorldMatrix * clipCoordinates};
	glm::vec3 worldCoordinates{glm::vec3{unprojectedWorldCoordinates} / unprojectedWorldCoordinates.w};

	glm::vec3 directionVector = worldCoordinates - camera.position;
	glm::vec3 planeNormalVector = {0, 1, 0};
	glm::vec3 planePoint = {0, 0, 0};

	float planeDotDirection = glm::dot(planeNormalVector, directionVector);
	float t = glm::dot(planeNormalVector, (planePoint - camera.position)) / planeDotDirection;

	glm::vec3 pointInWorld = camera.position + (t * directionVector);

	return {(!isnan(t) && t > 0), pointInWorld};
}

void World::on_mouse_move(GLFWwindow* window, double xpos, double ypos) {
	camera.pan(xpos, ypos);

	std::pair<bool, glm::vec3> result = World::getTileCoordFromWindowCoords(xpos, ypos);
	if (result.first) { //if t>0
		selectedTileCoordinates.colCoord = int(result.second.x + 0.5);
		selectedTileCoordinates.rowCoord = int(result.second.z + 0.5);
	} else {
		printf("bad tile selector calculation: x: %lf z: %lf\n", xpos, ypos);
	}
}

void World::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.mouseScroll = glm::vec2(xoffset, yoffset);
}

void World::on_window_resize(GLFWwindow* window, int width, int height) {
	Global::windowWidth = static_cast<size_t>(width);
	Global::windowHeight = static_cast<size_t>(height);
}

bool withinLevelBounds(glm::vec3 coords) {
	return (coords.x >= 0 && coords.x < Global::levelWidth) ||
		   (coords.z >= 0 && coords.z < Global::levelHeight);
}

void World::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	// single click select units
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	std::pair<bool, glm::vec3> targetLocation = World::getTileCoordFromWindowCoords(xpos, ypos);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (targetLocation.first && withinLevelBounds(targetLocation.second)) { //check for validity
			std::cout << "clicked " << targetLocation.second.x << " " << targetLocation.second.z << "\n";
			UnitManager::selectUnit(targetLocation.second);
		}
	};

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		if (targetLocation.first && withinLevelBounds(targetLocation.second)) { //check for validity
			UnitManager::attackTargetLocationWithSelectedUnits(targetLocation.second);
		}
	}


//	glm::vec3 coords = {selectedTileCoordinates.colCoord, 0, selectedTileCoordinates.rowCoord};
//	if (coords.x < 0 || coords.x + 1 > Global::levelWidth ||
//		coords.z < 0 || coords.z + 1 > Global::levelHeight)
//		return;
//	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
//
//		level.placeTile(Model::MeshType::GUN_TURRET, coords);
//		logger(LogLevel::INFO) << "Right click detected " << '\n';
//	}
//	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
//
//
//		if (m_dist(m_rng) < 0.5) {
//			Unit::spawn(Model::MeshType::FRIENDLY_RANGED_UNIT, coords, GamePieceOwner::PLAYER);
//		} else {
//			Unit::spawn(Model::MeshType::FRIENDLY_FIRE_UNIT, coords, GamePieceOwner::PLAYER);
//		}
//		logger(LogLevel::INFO) << "Left click detected " << '\n';
//	}
//	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
//		for (const auto& entity : Global::playerUnits) {
//			entity->rigidBody.setVelocity((coords - entity->rigidBody.getPosition()) / 5000.0f);
//		}
//	}
}
