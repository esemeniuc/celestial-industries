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
#include "ui.hpp"

namespace World {
	GLFWwindow* m_window;

	glm::vec2 m_screen;

	// Camera stuff
	Camera camera;

	// Selection
	Coord selectedTileCoordinates;
	bool shiftBeingHeld = false;

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
	Mix_Chunk* m_error_sound;

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
	if ((initted & flags) != flags) { // Bitwise & intentional; follows SDL docs
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

	m_error_sound = Mix_LoadWAV(audio_path("switch23.ogg"));

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
	level.particleShader = particleShader;

	std::shared_ptr<Texture> particleTexture = std::make_shared<Texture>();
	particleTexture->load_from_file(textures_path("oil.jpg"));
	if (!particleTexture->is_valid()) {
		throw "Particle texture failed to load!";
	}
	level.particleTexture = particleTexture;

	if (!initMeshTypes(Model::meshSources)) {
		logger(LogLevel::ERR) << "Failed to initialize renderers\n";
	}

	int windowWidth, windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	Global::windowWidth = static_cast<size_t>(windowWidth);
	Global::windowHeight = static_cast<size_t>(windowHeight);

	camera.position = {Config::CAMERA_START_POSITION_X, Config::CAMERA_START_POSITION_Y,
					   Config::CAMERA_START_POSITION_Z};

	Global::levelArray = level.levelLoader(pathBuilder({"data", "levels"}) + "GameLevel1.txt");
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
	auto temp2 = Unit::spawn(Model::MeshType::BALL, {startx, 0, startz}, GamePieceOwner::PLAYER);
//	temp2->moveTo(targetx, targetz);

	startx = 39, startz = 1;
	auto temp3 = Unit::spawn(Model::MeshType::BALL, {startx, 0, startz}, GamePieceOwner::PLAYER);
//	temp3->moveTo(UnitState::MOVE, targetx, targetz);

	startx = 20, startz = 20;
	auto temp4 = Unit::spawn(Model::MeshType::BALL, {startx, 0, startz}, GamePieceOwner::PLAYER);

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

	if (m_error_sound != nullptr) {
		Mix_FreeChunk(m_error_sound);
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
	camera.update((float)elapsed_ms);
	gameElapsedTime += elapsed_ms;

	if (selectedTileCoordinates.rowCoord >= 0 &&
		(unsigned long) selectedTileCoordinates.rowCoord < level.getLevelTraversalCostMap().size() &&
		selectedTileCoordinates.colCoord >= 0 &&
		(unsigned long) selectedTileCoordinates.colCoord < level.getLevelTraversalCostMap()[0].size()) {

		level.tileCursor->setPosition({selectedTileCoordinates.colCoord, 0, selectedTileCoordinates.rowCoord});
	}

	Global::playerResources += Global::playerResourcesPerSec * (elapsed_ms/1000);
	logger(LogLevel::DEBUG) << Global::playerResourcesPerSec * (elapsed_ms / 1000) << "\n";

	for (const auto& emitter : Global::emitters) {
		emitter->update(elapsed_ms);
	}

	Global::levelWithUnitsTraversalCostMap = Global::levelTraversalCostMap;
	AI::Manager::update(elapsed_ms);
	UnitManager::update(elapsed_ms);
	AttackManager::update(elapsed_ms);
	BuildingManager::update(elapsed_ms);

	for (const auto& tile : level.tiles) {
		tile->update(elapsed_ms);
	}
	for (const auto& entity : Global::playerUnits) {
		entity->animate(elapsed_ms);
	}
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

	for (const auto& emitter : Global::emitters) {
		emitter->render(projectionView, camera.position);
	}
	// Presenting
//	glfwSwapBuffers(m_window);
}

void World::play_mouse_click_sound() {
	Mix_PlayChannel(-1, m_mouse_click, 0);
}

void World::play_error_sound()
{
	Mix_PlayChannel(-1, m_error_sound, 0);
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

			// Shift
			{shiftBeingHeld,	   {GLFW_KEY_RIGHT_SHIFT, GLFW_KEY_LEFT_SHIFT}},
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
	if (result.first) {
		selectedTileCoordinates.colCoord = int(result.second.x + 0.5);
		selectedTileCoordinates.rowCoord = int(result.second.z + 0.5);
	} else {
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
	return (coords.x >= 0 && coords.x < Global::levelWidth) &&
		   (coords.z >= 0 && coords.z < Global::levelHeight);
}

void World::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	// single click select units
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	std::pair<bool, glm::vec3> targetLocation = World::getTileCoordFromWindowCoords(xpos, ypos);
	glm::vec3 coords = { selectedTileCoordinates.colCoord, 0, selectedTileCoordinates.rowCoord };
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (targetLocation.first && withinLevelBounds(targetLocation.second)) { //check for validity
			std::cout << "clicked " << targetLocation.second.x << " " << targetLocation.second.z << "\n";
			UnitManager::selectUnit(targetLocation.second);
		}

		std::vector<Model::MeshType> trees = { Model::MeshType::REDTREE, Model::MeshType::TREE, Model::MeshType::YELLOWTREE };
		if (withinLevelBounds(coords)) {
			switch (Ui::selectedBuilding) {
			case Ui::BuildingSelected::REFINERY: {
				const int refinerySize = 3;
				int numFriendlyTiles = level.numTilesOfOwnerInArea(GamePieceOwner::PLAYER, coords, refinerySize, refinerySize);
				int numGeysers = level.numTilesOfTypeInArea(Model::MeshType::GEYSER, coords, refinerySize, refinerySize);
				bool unpathableTiles = level.unpathableTilesInArea(coords, refinerySize, refinerySize);
				if (numFriendlyTiles > 0 || numGeysers == 0) {
					play_error_sound();
					break;
				}
				level.placeTile(Model::MeshType::REFINERY, coords, GamePieceOwner::PLAYER, refinerySize, refinerySize, numGeysers);
				break;
			}
			case Ui::BuildingSelected::SUPPLY_DEPOT:
				if (level.numTilesOfOwnerInArea(GamePieceOwner::PLAYER, coords) > 0 || level.unpathableTilesInArea(coords)) {
					play_error_sound();
					break;
				}
				level.placeTile(Model::MeshType::SUPPLY_DEPOT, coords);
				break;
			case Ui::BuildingSelected::COMMAND_CENTER: {
				const int width = 3;
				const int height = 2;
				if (level.numTilesOfOwnerInArea(GamePieceOwner::PLAYER, coords, width, height) > 0 || level.unpathableTilesInArea(coords, width, height)) {
					play_error_sound();
					break;
				}
				level.placeTile(Model::MeshType::COMMAND_CENTER, coords, GamePieceOwner::PLAYER, width, height);
				break;
			}
			case Ui::BuildingSelected::FACTORY: {
				const int width = 3;
				const int height = 2;
				if (level.numTilesOfOwnerInArea(GamePieceOwner::PLAYER, coords, width, height) > 0 || level.unpathableTilesInArea(coords, width, height)) {
					play_error_sound();
					break;
				}
				level.placeTile(Model::MeshType::FACTORY, coords, GamePieceOwner::PLAYER, width, height);
				break;
			}
			case Ui::BuildingSelected::GUN_TURRET:
				if (level.numTilesOfOwnerInArea(GamePieceOwner::PLAYER, coords) > 0 || level.unpathableTilesInArea(coords)) {
					play_error_sound();
					break;
				}
				level.placeTile(Model::MeshType::GUN_TURRET, coords);
				break;
			case Ui::BuildingSelected::NONE:
			default:
				break;
			}
		}
	};

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		if (targetLocation.first && withinLevelBounds(targetLocation.second)) { //check for validity
			UnitManager::attackTargetLocationWithSelectedUnits(targetLocation.second, shiftBeingHeld);
		}
	}
}
