// Header
#include <chrono>  // for high_resolution_clock
#include <queue>
#include <unordered_set>
#include "genericunit.hpp"
#include "logger.hpp"
#include "world.hpp"
#include "renderer.hpp"

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

typedef std::shared_ptr<Tile> pseudoUnit;
//std::unordered_set<pseudoUnit> units;
pseudoUnit unit1;
pseudoUnit unit2;
pseudoUnit unit3;
std::vector<std::vector<std::vector<GenericUnit>>> entityMap; //2d map of entities, where more than 1 entity can be in a

std::queue<std::pair<float, float>> interpPath1;
std::queue<std::pair<float, float>> interpPath2;
std::queue<std::pair<float, float>> interpPath3;

std::shared_ptr<Entity> ballPointer;
std::shared_ptr<Entity> ballPointer2;
std::pair<bool, std::vector<Coord>> path;

// World initialization
bool World::init(glm::vec2 screen) {
	//-------------------------------------------------------------------------
	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit()) {
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

	if (!initMeshTypes(Model::meshSources)) {
		logger(LogLevel::ERR) << "Failed to initialize renderers \n";
	}

	// TODO: Performance tanks and memory usage is very high for large maps. This is because the OBJ Data isn't being shared
	// thats a big enough change to merit its own ticket in milestone 2 though

	std::vector<std::vector<Model::MeshType>> levelArray = level.levelLoader(
			pathBuilder({"data", "levels"}) + "level1.txt");

	camera.position = {Config::CAMERA_START_POSITION_X, Config::CAMERA_START_POSITION_Y,
					   Config::CAMERA_START_POSITION_Z};

	level.init(levelArray, Model::meshRenderers);

	// test different starting points for the AI
	std::vector<std::vector<AStarNode>> costMap = level.getLevelTraversalCostMap();
	//benchmark a*
	{
		auto start = std::chrono::high_resolution_clock::now();
		AI::aStar::a_star(costMap, 1, 19, 1, 11, 25);
		AI::aStar::a_star(costMap, 1, 1, 1, 11, 25);
		AI::aStar::a_star(costMap, 1, 1, 39, 11, 25);
		auto finish = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> elapsed = finish - start;
		std::cout << "Elapsed time: " << elapsed.count() << " s\n";
	}


	entityMap.resize(levelArray.size());
	for (size_t i = 0; i < levelArray.size(); i++) {
		entityMap[i].resize(levelArray[i].size());
	}
	//display a path
	int startx = 12, startz = 27;
	int targetx = 10, targetz = 10;
	std::vector<Coord> path1 =
			AI::aStar::a_star(costMap, 1, 12, 27, targetx, targetz).second;
	level.displayPath(path1);
	GenericUnit temp;
	temp.translate({27, 0, 11});
	temp.setTargetPath(path1);
	entityMap[startx][startz].push_back(temp);
//	interpPath1 = AI::aStar::createInterpolatedPath(path1);

	//render the path
	unit1 = std::make_shared<Tile>(Model::meshRenderers[Model::MeshType::BALL]);
	unit1->translate({27, 0, 11});
	level.tiles.push_back({{unit1}});
//	units.insert(unit1);

	//wall example
	//display a path
	std::vector<Coord> path2 = AI::aStar::a_star(costMap, 1, 19, 40, targetx, targetz).second;
	std::cout << "path2 length: " << path2.size() << '\n';
	level.displayPath(path2);
	interpPath2 = AI::aStar::createInterpolatedPath(path2);

	//render the path
	unit2 = std::make_shared<Tile>(Model::meshRenderers[Model::MeshType::WALL]);
	unit2->translate({39, 0, 19});
	level.tiles.push_back({{unit2}});

	//mining tower example
	//display a path
	std::vector<Coord> path3 =
			AI::aStar::a_star(costMap, 1, 1, 40, targetx, targetz).second;
	std::cout << "path3 length: " << path3.size() << '\n';
	level.displayPath(path3);
	interpPath3 = AI::aStar::createInterpolatedPath(path3);

	//render the path
	unit3 = std::make_shared<Tile>(Model::meshRenderers[Model::MeshType::MINING_TOWER]);
	unit3->translate({39, 0, 1});
	level.tiles.push_back({{unit3}});

	ballPointer = std::make_shared<Entity>(Model::MeshType::BALL);
	ballPointer2 = std::make_shared<Entity>(Model::MeshType::BALL);

	//display a path
	//std::pair<bool, std::vector<Coord>> path =
	//		AI::aStar::a_star(costMap, 1, 12, 27, (int) mapSize / 2, (int) mapSize / 2);
	//level.displayPath(path.second);

	selectedTileCoordinates.rowCoord = (int) levelArray.size() / 2;
	selectedTileCoordinates.colCoord = (int) levelArray.size() / 2;
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

bool World::update(float elapsed_ms) {
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

	for (auto& dim1 : entityMap) {
		for (auto& dim2 : dim1) {
			for (auto& dim3 : dim2) {
				dim3.move(elapsed_ms);

			}
		}
	}

	//display interpolated moves for ball
//	for (const auto& unit : units) {
//		if (!interpPath1.empty()) {
//			auto coord = interpPath1.front();
//			unit->translate({coord.second, 0, coord.first});
//			interpPath1.pop();
//		}
//	}
	ballPointer->translate(glm::vec3(0.01, 0.0, 0.01));
	ballPointer2->translate(glm::vec3(-0.01, 0.0, -0.01));
	ballPointer2->animate(elapsed_ms);


	if (!interpPath1.empty()) {
		auto coord = interpPath1.front();
		unit1->translate({coord.second, 0, coord.first});
		interpPath1.pop();
	}
	if (!interpPath2.empty()) {
		auto coord = interpPath2.front();
		unit2->translate({coord.second, 0, coord.first});
		interpPath2.pop();
	}

	if (!interpPath3.empty()) {
		auto coord = interpPath3.front();
		unit3->translate({coord.second, 0, coord.first});
		interpPath3.pop();
	}

	for (const auto& turret : level.guntowers) {
		turret->update(elapsed_ms);
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

	for (auto renderer : Model::meshRenderers) {
		renderer.second->render(projectionView);
	}


	// make skybox rotate by 0.001 * pi/4 radians around y axis, every frame
//	float y_rotation = 0.005 * glm::quarter_pi<float>();
//	m_skybox.setRotation(glm::vec3(0.0, y_rotation, 0.0));
//	m_skybox.applyTransformations();
	m_skybox.getCameraPosition(camera.position);
	m_skybox.draw(projection * view * m_skybox.getModelMatrix());

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
//	logger(LogLevel::DEBUG) << "X-pos: " << xpos << ", Y-pos: " << ypos << '\n';
	camera.pan((int) xpos, (int) ypos);

	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
//    char debugMessage[10000];

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

//        snprintf(debugMessage, 10000, "FB mouse: %d, %d | Clipcoords: <%f, %f> | Camera surface coords: <%f, %f, %f> | Coordinates in world space: %f, %f, %f | Selected tile: <%d, %d>\n",
//                 computedFbX, computedFbY,
//                 clipCoordinates[0], clipCoordinates[1],
//                 worldCoordinates.x, worldCoordinates.y, worldCoordinates.z,
//                 pointInWorld.x, pointInWorld.y, pointInWorld.z,
//                 selectedTileCoordinates.rowCoord, selectedTileCoordinates.colCoord
//        );
//		logger(LogLevel::DEBUG) << debugMessage;
	}


}

void World::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.mouseScroll = glm::vec2(xoffset, yoffset);
}

