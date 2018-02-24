// Header
#include "world.hpp"
#include "basicentity.hpp"

// Same as static in c, local to compilation unit
namespace {
	const size_t TILE_WIDTH = 10;

	namespace {
		void glfw_err_cb(int error, const char* desc) {
			fprintf(stderr, "%d: %s", error, desc);
		}
	}
}

World::World() {
	// Seeding rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World() {

}

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
	m_window = glfwCreateWindow((int) screen.x, (int) screen.y, "A1 Assignment", nullptr, nullptr);
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
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	// setup skybox
	bool skyboxLoaded = loadSkybox("skybox.obj", "skybox");
	if (!skyboxLoaded) {
		return false;
	}

	// WHY WASNT THIS ENABLED BEFORE?!
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

    /*
    In the context of SubObjects the number to the left of the filename indicates which of the other objs the obj is dependent on. If it's -1 then that
    means none. This is used to figure out what the transformation tree is for animating
    */
	std::vector<std::pair<TileType, std::vector<SubObjectSource>>> tiles = {
            {SAND_1,       {{"sand1.obj", -1}} },
            {SAND_2,       { {"sand2.obj", -1}} },
            {SAND_3,       {{"sand3.obj", -1}} },
            {WALL,         {{"wall.obj", -1}} },
            {BRICK_CUBE,   {{"brickCube.obj", -1}} },
            {MINING_TOWER, {{"miningTower.obj", -1}} },
            {PHOTON_TOWER, {{"photonTower.obj", -1}} },
            {GUN_TURRET,   {{"TurretBase.obj", -1}, {"TurretTop.obj", 0}, {"TurretGunsLeft.obj", 1}, {"TurretGunsRight.obj", 1}} },
	};

    // Load shader for default tiles
    objShader = std::make_shared<Shader>();
    if (!objShader->load_from_file(shader_path("objrenderable.vs.glsl"), shader_path("objrenderable.fs.glsl"))) {
        logger(LogLevel::ERR) << "Failed to load obj shader!" << Logger::endl;
        return false;
    }

	// TODO: Performance tanks and memory usage is very high for large maps. This is because the OBJ Data isnt being shared
	// thats a big enough change to merit its own ticket in milestone 2 though
	std::vector<std::vector<int>> levelArray;
	int mapSize = 10;
	for (size_t i = 0; i < (size_t) mapSize; i++) {
		std::vector<int> row;
		for (size_t j = 0; j < (size_t) mapSize; j++) {
			if (j == 2 && i == 2)row.push_back(GUN_TURRET);
			/*else if (i % 3 == 0 && j % 3 == 0)row.push_back(PHOTON_TOWER);
			else if (i % 5 == 0 && j % 2 == 0)row.push_back(BRICK_CUBE);*/
			else row.push_back(SAND_1);
            
            //row.push_back(SAND_1);
		}
		levelArray.push_back(row);
	}
	camera.position = {mapSize / 2, 10, mapSize / 2};
    logger(LogLevel::DEBUG) << "Loading level... " << Logger::endl;
	level.init(levelArray, tiles, objShader);
    logger(LogLevel::DEBUG) << "Level loading complete." << Logger::endl;

	selectedTile = {mapSize / 2, mapSize / 2};




	OBJ::Data ball;
	if (!OBJ::Loader::loadOBJ(pathBuilder({"data", "models"}), "ball.obj", ball)) {
        logger(LogLevel::ERR) << "No ball, no game" << Logger::endl;
		return false;
	}
    auto ballMeshResult = objToMesh(ball);
    if (!ballMeshResult.first) {
        logger(LogLevel::ERR) << "Failed to convert ball to meshes" << Logger::endl;
        return false;
    }
    auto ballMeshes = ballMeshResult.second;
	for (size_t i = 0; i < (size_t) mapSize / 3; i++) {
		BasicEntity ballEntity;
		ballEntity.init(ballMeshes, objShader);
		ballEntity.translate({0, 2, 0});
		level.basicEntities.push_back(ballEntity);
	}
	return true;
}

// skybox
bool World::loadSkybox(std::string skyboxFilename, std::string skyboxTextureFolder) {
	bool success = true;
	OBJ::Data skyboxObj;

	std::string geometryPath = pathBuilder({"data", "models"});
	std::string texturePath = pathBuilder({"data", "textures", skyboxTextureFolder});

	success &= OBJ::Loader::loadOBJ(geometryPath, skyboxFilename, skyboxObj);
	if (!success) {
        logger(LogLevel::ERR) << "Failed to load skybox" << Logger::endl;
		return false;
	}

	success &= m_skybox.init(skyboxObj);
	if (!success) {
        logger(LogLevel::ERR) << "Failed to initilize skybox" << Logger::endl;
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
//	glm::vec2 screen = glm::vec2((float)w, (float)h);
	camera.update(elapsed_ms);
    level.update(elapsed_ms);
	for (size_t i = 0; i < level.basicEntities.size(); i++) {
		// Yes we know its terribly inefficient as is, but this is more of a "it works" kindda demo rather than the final
		// AI driven logic
		if (level.basicEntities[i].position.x < 2) {
			level.basicEntities[i].moveTo({50, 2, i * 3});
		}
		if (level.basicEntities[i].position.x > 48) {
			level.basicEntities[i].moveTo({0, 2, level.basicEntities[i].position.z});
		}
		level.basicEntities[i].update(elapsed_ms);
	}
	total_time += elapsed_ms;
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

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Celestial Industries";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// Clearing backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = {47.0 / 256.0, 61.0 / 256.0, 84.0 / 256.0};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = camera.getProjectionMatrix(m_screen.x, m_screen.y);
	glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projectionView = projection * view;

    for (auto tileRenderer : level.tileRenderers) {
        tileRenderer.second->render(projectionView);
    }

	for (auto basicEntity : level.basicEntities) {
		basicEntity.draw(projectionView);
	}

	m_skybox.draw(projection * view * m_skybox.model);

	// Presenting
	glfwSwapBuffers(m_window);
}

void World::move_cursor_up() {
	selectedTile[1]--;
	printf("Selected tile: %d, %d\n", selectedTile[0], selectedTile[1]);
}

void World::move_cursor_down() {
	selectedTile[1]++;
	printf("Selected tile: %d, %d\n", selectedTile[0], selectedTile[1]);
}

void World::move_cursor_left() {
	selectedTile[0]--;
	printf("Selected tile: %d, %d\n", selectedTile[0], selectedTile[1]);
}

void World::move_cursor_right() {
	selectedTile[0]++;
	printf("Selected tile: %d, %d\n", selectedTile[0], selectedTile[1]);
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
	// Handle the mouse movement here
}

void World::on_mouse_scroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.mouseScroll = glm::vec2(xoffset, yoffset);
}

