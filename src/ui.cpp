//
// Created by eric on 4/4/18.
//

#include "ui.hpp"
#include "global.hpp" //for window size
#include "unit.hpp" //for spawning
#include "building.hpp" //for spawning
#include "world.hpp" //for key callbacks
#include "config.hpp" //for font file path
#include "unitmanager.hpp" //for unit selection info

#include "IconsFontAwesome5.h" //for game icons
#include "entityinfo.h"

ImVec2 operator+(const ImVec2& a, const ImVec2& b) {
	return {a.x + b.x, a.y + b.y};
}

ImVec2 operator-(const ImVec2& a, const ImVec2& b) {
	return {a.x - b.x, a.y - b.y};
}

std::ostream& operator<<(std::ostream& os, const ImVec2& vec2) {
	os << "x: " << vec2.x << " y: " << vec2.y;
	return os;
}

ImVec2 abs(const ImVec2& a) {
	return {abs(a.x), abs(a.y)};
}

namespace Ui {
	void imguiSetup() {
		// Setup ImGui binding
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void) io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
		ImGui_ImplGlfwGL3_Init(g_Window, true, "#version 410");

		// Setup style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'misc/fonts/README.txt' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);

		//IconFont stuff
//		io.Fonts->AddFontDefault(); dont use proggy font
		io.Fonts->AddFontFromFileTTF(Config::KENNEY_FUTURE_FONT_FILE_PATH, 16.0f);

		// merge in icons from Font Awesome
		static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF(Config::FONTAWESOME_FILE_PATH, 16.0f, &icons_config, icons_ranges);
		// use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid
	}


	void imguiGenerateScreenObjects() {
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		int windowWidth, windowHeight;
		glfwGetWindowSize(g_Window, &windowWidth, &windowHeight);
		ImGui_ImplGlfwGL3_NewFrame();
//		ImGui::ShowDemoWindow();

		//TODO: dont allow pan callback if scrolling

		if (ImGui::IsMouseDragging()) { //for selection window
			ImVec2 startClick = ImGui::GetIO().MouseClickedPos[0]; //0 for main mouse button
			ImVec2 endClick = ImGui::GetMousePos();

			unitSelectionSize = abs(endClick - startClick);
			topLeft = getSelectionBoxStartPos(startClick, endClick);
			bottomRight = topLeft + unitSelectionSize;
			ImGui::SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(std::numeric_limits<float>::max(),
																	 std::numeric_limits<float>::max()));

			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f); //make a square box
			ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0, 0)); //make small
			ImGui::SetNextWindowPos(topLeft); //window always starts from top left corner
			ImGui::SetNextWindowSize(unitSelectionSize);
//			std::cout << "start: " << startClick << " end: " << endClick << " size: " << selectionSize << " tl: "
//					  << topLeft << '\n';
			ImGui::SetNextWindowBgAlpha(0.3f); // Transparent background
			ImGui::Begin("Game selection overlay", nullptr,
						 ImGuiWindowFlags_NoTitleBar |
						 ImGuiWindowFlags_NoResize |
						 ImGuiWindowFlags_NoMove |
						 ImGuiWindowFlags_NoSavedSettings |
						 ImGuiWindowFlags_NoFocusOnAppearing |
						 ImGuiWindowFlags_NoNav |
						 ImGuiWindowFlags_NoInputs |
						 ImGuiWindowFlags_NoScrollbar);

			ImGui::End();
			ImGui::PopStyleVar(); //size
			ImGui::PopStyleVar(); //square
		} else if (ImGui::IsMouseDragging(1)) {
			std::cout << "dragging2\n";
			std::cout << ImGui::GetMouseDragDelta(1).x << ' ' << ImGui::GetMouseDragDelta(1).y << "\n";

		}


		{ //resources counter at top right
			const float DISTANCE = 10.0f;
			ImVec2 window_pos = ImVec2(ImGui::GetIO().DisplaySize.x - DISTANCE, DISTANCE);
			ImVec2 window_pos_pivot = ImVec2(1.0f, 0.0f);
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
			ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoSavedSettings |
											   ImGuiWindowFlags_NoResize |
											   ImGuiWindowFlags_NoCollapse |
											   ImGuiWindowFlags_NoMove |
											   ImGuiWindowFlags_NoTitleBar |
											   ImGuiWindowFlags_AlwaysAutoResize |
											   ImGuiWindowFlags_NoFocusOnAppearing |
											   ImGuiWindowFlags_NoNav);

			ImGui::Text(ICON_FA_HAND_HOLDING_USD " Resources:     %8d", Global::playerResources);
			ImGui::Text(" " ICON_FA_DOLLAR_SIGN "  Resource Rate: %8d", Global::playerResourcesPerSec);
			ImGui::Text(ICON_FA_WAREHOUSE " Supply:        %8d/%d", Global::playerCurrentSupply,
						Global::playerMaxSupply);
			ImGui::End();
		}

		{ //entity info
			ImGui::SetNextWindowSize(ImVec2(windowWidth - spawnWindowWidth, uiHeight));
			ImGui::SetNextWindowPos(ImVec2(0, windowHeight - uiHeight));
			ImGui::Begin("Entity Info", nullptr, ImGuiWindowFlags_NoSavedSettings |
												 ImGuiWindowFlags_NoResize |
												 ImGuiWindowFlags_NoCollapse |
												 ImGuiWindowFlags_NoMove |
												 ImGuiWindowFlags_NoTitleBar);

			//need this for display of selected units
			ImVec2 topRight = ImVec2(topLeft.x + unitSelectionSize.x, topLeft.y); //not calculated during selection
			ImVec2 bottomLeft = ImVec2(topLeft.x, topLeft.y + unitSelectionSize.y);

			std::pair<bool, glm::vec3> topLeftTileCoord = World::getTileCoordFromWindowCoords(topLeft.x, topLeft.y);
			std::pair<bool, glm::vec3> bottomRightTileCoord = World::getTileCoordFromWindowCoords(bottomRight.x,
																								  bottomRight.y);

			std::pair<bool, glm::vec3> topRightTileCoord = World::getTileCoordFromWindowCoords(topRight.x, topRight.y);
			std::pair<bool, glm::vec3> bottomLeftTileCoord = World::getTileCoordFromWindowCoords(bottomLeft.x,
																								 bottomLeft.y);
			if (topLeftTileCoord.first && topRightTileCoord.first &&
				bottomRightTileCoord.first && bottomLeftTileCoord.first) { //check since might not be invertible

				UnitManager::selectUnitsInTrapezoid(topLeftTileCoord.second, topRightTileCoord.second,
													bottomLeftTileCoord.second, bottomRightTileCoord.second);
				std::cout << "selected: " << UnitManager::selectedUnits.size() << "\t|| " <<
						  topLeftTileCoord.second.x << ' ' << topLeftTileCoord.second.z << ' ' <<
						  bottomRightTileCoord.second.x << ' ' << bottomRightTileCoord.second.z << ' ' <<
						  '\n';
			}

			if (UnitManager::selectedUnits.size() == 1) {
				std::shared_ptr<Entity> unit = UnitManager::selectedUnits.front();
				ImGui::Text("Unit: %s\n", EntityInfo::nameLookupTable[unit->meshType]);
				ImGui::Text("Health: %.f/%d\n", unit->aiComp.currentHealth, unit->aiComp.totalHealth);
				ImGui::Text("Damage: %d\n", unit->unitComp.attackDamage);
				ImGui::Text("Attack Range: %d\n", unit->unitComp.attackRange);
				ImGui::Text("Attack Speed: %d\n", unit->unitComp.attackSpeed);
				ImGui::Text("Owner: %d\n", unit->aiComp.owner);
				ImGui::Text("Type: %d:", unit->aiComp.type);
			} else if (UnitManager::selectedUnits.size() > 1) {
				for (const auto& unit : UnitManager::selectedUnits) {
//draw something
				}
			}


			ImGui::End();
		}

		{    //spawn selector
			ImGui::SetNextWindowSize(ImVec2(spawnWindowWidth, uiHeight));
			ImGui::SetNextWindowPos(ImVec2(windowWidth - spawnWindowWidth, windowHeight - uiHeight));
			ImGui::Begin("Construct Building", nullptr, ImGuiWindowFlags_NoSavedSettings |
														ImGuiWindowFlags_NoResize |
														ImGuiWindowFlags_NoCollapse |
														ImGuiWindowFlags_NoMove);

			switch (spawnWindowState) {
				case SpawnWindowState::SPAWN_SELECTOR: {
					if (ImGui::Button(ICON_FA_BUILDING " Economic")) {
						spawnWindowState = SpawnWindowState::SPAWN_ECONOMIC_BUILDINGS;
					}

					if (ImGui::Button(ICON_FA_SHIELD_ALT " Defence")) {
						spawnWindowState = SpawnWindowState::SPAWN_DEFENSIVE_BUILDINGS;
					}
					break;
				}
				case SpawnWindowState::SPAWN_DEFENSIVE_BUILDINGS : {

					if (ImGui::Button("Gun Turret")) {
						Building::spawn(Model::GUN_TURRET, glm::vec3(10, 0, 10),
										GamePieceOwner::PLAYER);
					}

					//back button alignment
					ImGui::SetCursorPos(ImVec2(spawnWindowWidth - ImGui::GetFontSize() * 5,
											   uiHeight - ImGui::GetFontSize() * 3));
					if (ImGui::Button("Back")) {
						spawnWindowState = SpawnWindowState::SPAWN_SELECTOR;
					}
					break;
				}
				case SpawnWindowState::SPAWN_ECONOMIC_BUILDINGS : {

//					if (ImGui::Button("Command Center")) {
//						Building::spawn(Model::, glm::vec3(35, 0, 35),
//										GamePieceOwner::PLAYER);
//					}

					if (ImGui::Button("Refinery")) {
						Building::spawn(Model::MINING_TOWER, glm::vec3(15, 0, 15),
										GamePieceOwner::PLAYER);
					}

					if (ImGui::Button("Supply Depot")) {
						Building::spawn(Model::MeshType::SUPPLY_DEPOT, glm::vec3(25, 0, 25),
										GamePieceOwner::PLAYER);
					}

					ImGui::SetCursorPos(ImVec2(spawnWindowWidth - ImGui::GetFontSize() * 5,
											   uiHeight - ImGui::GetFontSize() * 3));
					if (ImGui::Button("Back")) {
						spawnWindowState = SpawnWindowState::SPAWN_SELECTOR;
					}
					break;
				}
			}

			ImGui::End();
		}


		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(g_Window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//		glClear(GL_COLOR_BUFFER_BIT);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(g_Window);
	}

	ImVec2 getSelectionBoxStartPos(const ImVec2& startClickPos, const ImVec2& endClickPos) {
		ImVec2 topLeft;
		if (startClickPos.x <= endClickPos.x && startClickPos.y <= endClickPos.y) {
			topLeft = startClickPos; //normal top left to bottom right case
		} else if (startClickPos.x >= endClickPos.x && startClickPos.y >= endClickPos.y) {
			topLeft = endClickPos; //the start at bottom right, end at top left case
		} else if (startClickPos.x >= endClickPos.x && startClickPos.y <= endClickPos.y) {
			topLeft = ImVec2(endClickPos.x, startClickPos.y); //the start at top right, end at bottom left case
		} else if (startClickPos.x <= endClickPos.x && startClickPos.y >= endClickPos.y) {
			topLeft = ImVec2(startClickPos.x, endClickPos.y); //the start at bottom left, end at top right case
		}
		return topLeft;
	}

	bool createWindow() {
		//-------------------------------------------------------------------------
		// GLFW / OGL Initialization
		// Core Opengl 3.
		auto glfw_err_cb = [](int error, const char* desc) {
			fprintf(stderr, "Error %d: %s\n", error, desc);
		};
		glfwSetErrorCallback(glfw_err_cb);
		if (!glfwInit()) {
			logger(LogLevel::ERR) << "Failed to initialize GLFW\n";
			return false;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
		glfwWindowHint(GLFW_RESIZABLE, 1);
		g_Window = glfwCreateWindow(Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOW_HEIGHT, Config::WINDOW_TITLE,
									nullptr, nullptr);
		if (g_Window == nullptr)
			return false;

		glfwMakeContextCurrent(g_Window);
		glfwSwapInterval(1); // vsync

		// Load OpenGL function pointers
		gl3wInit();

		// WHY WASNT THIS ENABLED BEFORE?!
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		return true;
	}


	void imguiDrawLaunchMenu() {
		while (Global::gameState == GameState::START_MENU && !glfwWindowShouldClose(Ui::getWindow())) {
			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			static bool showTutorial = false;
			{ //draw launch menu
				ImGui::SetNextWindowPosCenter();
				ImGui::Begin("Launch Menu", nullptr, ImGuiWindowFlags_NoSavedSettings |
													 ImGuiWindowFlags_NoResize |
													 ImGuiWindowFlags_NoCollapse |
													 ImGuiWindowFlags_NoMove |
													 ImGuiWindowFlags_NoTitleBar |
													 ImGuiWindowFlags_AlwaysAutoResize |
													 ImGuiWindowFlags_NoNav);

				//display game logo as part of start up screen
				ImGui::Text("Celestial Industries");
				ImTextureID gameLogo = (void*) World::logoTexture.id;				
				ImGui::Image(gameLogo, ImVec2(World::logoTexture.width, World::logoTexture.height));

				if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Start")) {
					Global::gameState = GameState::PLAY;
				}

				if (ImGui::Button(ICON_FA_QUESTION_CIRCLE " Tutorial")) {
					showTutorial = true;
				}

				if (ImGui::Button(ICON_FA_TIMES_CIRCLE " Quit")) {
					Global::gameState = GameState::QUIT;
				}
				ImGui::End();
			}


			if (showTutorial) {
				ImGui::SetNextWindowPosCenter();
				ImGui::Begin("How To Play", &showTutorial);
				ImGui::NewLine();
				ImGui::TextWrapped("Story\n\n"
								   "It is the distant future. You are working for Celestial Industries,"
								   " the omnipresent megacorporation that runs human civilization, and have been"
								   " tasked with overseeing a robotic colony development team. You are offered a promotion"
								   " if you can get the colony to produce [Amount(s)] of [Resource(s)] per"
								   " [Time unit], at which point you’d get to supervise a much cooler planet.");

				ImGui::NewLine();
				ImGui::Text("Movement Keys:\n");
				ImGui::Text("W\t\t\t\t\t\tMove camera up\n");
				ImGui::Text("S\t\t\t\t\t\tMove camera down\n");
				ImGui::Text("A\t\t\t\t\t\tMove camera left\n");
				ImGui::Text("D\t\t\t\t\t\tMove camera right\n");

				ImGui::NewLine();
				ImGui::Text("P\t\t\t\t\t\tPause game\n");
				ImGui::Text("Esc\t\t\t\t\tQuit game\n");

				ImGui::NewLine();
				ImGui::Text("Left click/drag\t\tSelect Units\n");
				ImGui::Text("Right click\t\t\t\t\tMove/Attack\n");

				ImGui::NewLine();
				if (ImGui::Button("Close")) {
					showTutorial = false;
				}

				ImGui::End();
			}

			if (ImGui::GetIO().KeysDown[GLFW_KEY_ESCAPE]) {
				Global::gameState = GameState::QUIT;
			}

			// Rendering
			int display_w, display_h;
			glfwGetFramebufferSize(g_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(g_Window);
		}
	}

	void imguiDrawPauseMenu() {
		while (Global::gameState == GameState::PAUSED && !glfwWindowShouldClose(Ui::getWindow())) {
			glfwPollEvents();
			ImGui_ImplGlfwGL3_NewFrame();

			{ //draw menu
				ImGui::SetNextWindowPosCenter();
				ImGui::Begin("Pause Menu", nullptr, ImGuiWindowFlags_NoSavedSettings |
													ImGuiWindowFlags_NoResize |
													ImGuiWindowFlags_NoCollapse |
													ImGuiWindowFlags_NoMove |
													ImGuiWindowFlags_NoTitleBar |
													ImGuiWindowFlags_AlwaysAutoResize |
													ImGuiWindowFlags_NoNav);
				ImGui::Text(ICON_FA_PAUSE_CIRCLE " Game Paused\n\n\n");
				ImGui::Text("Continue playing?");
				if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Resume")) {
					Global::gameState = GameState::PLAY;
				}

				if (ImGui::Button(ICON_FA_TIMES_CIRCLE " Quit")) {
					Global::gameState = GameState::QUIT;
				}

				ImGui::End();
			}

			if (ImGui::GetIO().KeysDown[GLFW_KEY_ESCAPE]) {
				Global::gameState = GameState::PLAY;
			}

			// Rendering
			int display_w, display_h;
			glfwGetFramebufferSize(g_Window, &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(g_Window);
		}
	}


	GLFWwindow* getWindow() {
		return g_Window;
	}

	// OpenGL3 Render function.
// (this used to be set in io.RenderDrawListsFn and called by ImGui::Render(), but you can now call this directly from your main loop)
// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so.
	void ImGui_ImplGlfwGL3_RenderDrawData(ImDrawData* draw_data) {
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO& io = ImGui::GetIO();
		int fb_width = (int) (io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int) (io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);

		// Backup GL state
		GLenum last_active_texture;
		glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*) &last_active_texture);
		glActiveTexture(GL_TEXTURE0);
		GLint last_program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_sampler;
		glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
		GLint last_array_buffer;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		GLint last_element_array_buffer;
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		GLint last_vertex_array;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
		GLint last_polygon_mode[2];
		glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
		GLint last_viewport[4];
		glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4];
		glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
		GLenum last_blend_src_rgb;
		glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*) &last_blend_src_rgb);
		GLenum last_blend_dst_rgb;
		glGetIntegerv(GL_BLEND_DST_RGB, (GLint*) &last_blend_dst_rgb);
		GLenum last_blend_src_alpha;
		glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*) &last_blend_src_alpha);
		GLenum last_blend_dst_alpha;
		glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*) &last_blend_dst_alpha);
		GLenum last_blend_equation_rgb;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*) &last_blend_equation_rgb);
		GLenum last_blend_equation_alpha;
		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*) &last_blend_equation_alpha);
		GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
		GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei) fb_width, (GLsizei) fb_height);
		const float ortho_projection[4][4] =
				{
						{2.0f / io.DisplaySize.x, 0.0f,                     0.0f,  0.0f},
						{0.0f,                    2.0f / -io.DisplaySize.y, 0.0f,  0.0f},
						{0.0f,                    0.0f,                     -1.0f, 0.0f},
						{-1.0f,                   1.0f,                     0.0f,  1.0f},
				};
		glUseProgram(g_ShaderHandle);
		glUniform1i(g_AttribLocationTex, 0);
		glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
		glBindSampler(0, 0); // Rely on combined texture/sampler state.

		// Recreate the VAO every time
		// (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
		GLuint vao_handle = 0;
		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);
		glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
		glEnableVertexAttribArray(g_AttribLocationPosition);
		glEnableVertexAttribArray(g_AttribLocationUV);
		glEnableVertexAttribArray(g_AttribLocationColor);
		glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
							  (GLvoid*) IM_OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
							  (GLvoid*) IM_OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
							  (GLvoid*) IM_OFFSETOF(ImDrawVert, col));

		// Draw
		for (int n = 0; n < draw_data->CmdListsCount; n++) {
			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr) cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
						 (const GLvoid*) cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr) cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
						 (const GLvoid*) cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback) {
					pcmd->UserCallback(cmd_list, pcmd);
				} else {
					glBindTexture(GL_TEXTURE_2D, (GLuint) (intptr_t) pcmd->TextureId);
					glScissor((int) pcmd->ClipRect.x, (int) (fb_height - pcmd->ClipRect.w),
							  (int) (pcmd->ClipRect.z - pcmd->ClipRect.x), (int) (pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei) pcmd->ElemCount,
								   sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
		glDeleteVertexArrays(1, &vao_handle);

		// Restore modified GL state
		glUseProgram(last_program);
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindSampler(0, last_sampler);
		glActiveTexture(last_active_texture);
		glBindVertexArray(last_vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, (GLenum) last_polygon_mode[0]);
		glViewport(last_viewport[0], last_viewport[1], (GLsizei) last_viewport[2], (GLsizei) last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei) last_scissor_box[2],
				  (GLsizei) last_scissor_box[3]);
	}

	static const char* ImGui_ImplGlfwGL3_GetClipboardText(void* user_data) {
		return glfwGetClipboardString((GLFWwindow*) user_data);
	}

	static void ImGui_ImplGlfwGL3_SetClipboardText(void* user_data, const char* text) {
		glfwSetClipboardString((GLFWwindow*) user_data, text);
	}

	void ImGui_ImplGlfw_CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
		//make sure to call the world mouse callback only game world and not ui
//		if (ypos < Global::windowHeight - uiHeight) {
		World::on_mouse_move(window, xpos, ypos); //cant do check because of camera panning
//		}
	}

	void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		if (action == GLFW_PRESS && button >= 0 && button < 3) {
			g_MouseJustPressed[button] = true;
			// play mouse click sound
			World::play_mouse_click_sound();
		}

		//make sure to call the world mouse callback only game world and not ui
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (ypos < Global::windowHeight - uiHeight) {
			World::on_mouse_button(window, button, action, mods);
		}
	}

	void ImGui_ImplGlfw_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += (float) xoffset;
		io.MouseWheel += (float) yoffset;

		World::on_mouse_scroll(window, xoffset, yoffset);
	}

	void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		ImGuiIO& io = ImGui::GetIO();
		if (action == GLFW_PRESS)
			io.KeysDown[key] = true;
		if (action == GLFW_RELEASE)
			io.KeysDown[key] = false;

		(void) mods; // Modifiers are not reliable across systems
		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];


		if (Global::gameState == GameState::PLAY) { //only allow world key call backs when playing
			World::on_key(window, key, scancode, action, mods);
		}
	}

	void ImGui_ImplGlfw_CharCallback(GLFWwindow*, unsigned int c) {
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter((unsigned short) c);
	}

	bool ImGui_ImplGlfwGL3_CreateFontsTexture() {
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width,
									 &height);
		// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders.
		// If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

		// Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &g_FontTexture);
		glBindTexture(GL_TEXTURE_2D, g_FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void*) (intptr_t) g_FontTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture);

		return true;
	}

	bool ImGui_ImplGlfwGL3_CreateDeviceObjects() {
		// Backup GL state
		GLint last_texture, last_array_buffer, last_vertex_array;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		const GLchar* vertex_shader =
				"uniform mat4 ProjMtx;\n"
				"in vec2 Position;\n"
				"in vec2 UV;\n"
				"in vec4 Color;\n"
				"out vec2 Frag_UV;\n"
				"out vec4 Frag_Color;\n"
				"void main()\n"
				"{\n"
				"	Frag_UV = UV;\n"
				"	Frag_Color = Color;\n"
				"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
				"}\n";

		const GLchar* fragment_shader =
				"uniform sampler2D Texture;\n"
				"in vec2 Frag_UV;\n"
				"in vec4 Frag_Color;\n"
				"out vec4 Out_Color;\n"
				"void main()\n"
				"{\n"
				"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
				"}\n";

		const GLchar* vertex_shader_with_version[2] = {g_GlslVersion, vertex_shader};
		const GLchar* fragment_shader_with_version[2] = {g_GlslVersion, fragment_shader};

		g_ShaderHandle = glCreateProgram();
		g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
		g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(g_VertHandle, 2, vertex_shader_with_version, NULL);
		glShaderSource(g_FragHandle, 2, fragment_shader_with_version, NULL);
		glCompileShader(g_VertHandle);
		glCompileShader(g_FragHandle);
		glAttachShader(g_ShaderHandle, g_VertHandle);
		glAttachShader(g_ShaderHandle, g_FragHandle);
		glLinkProgram(g_ShaderHandle);

		g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
		g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
		g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
		g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
		g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

		glGenBuffers(1, &g_VboHandle);
		glGenBuffers(1, &g_ElementsHandle);

		ImGui_ImplGlfwGL3_CreateFontsTexture();

		// Restore modified GL state
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindVertexArray(last_vertex_array);

		return true;
	}

	void ImGui_ImplGlfwGL3_InvalidateDeviceObjects() {
		if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
		if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
		g_VboHandle = g_ElementsHandle = 0;

		if (g_ShaderHandle && g_VertHandle) glDetachShader(g_ShaderHandle, g_VertHandle);
		if (g_VertHandle) glDeleteShader(g_VertHandle);
		g_VertHandle = 0;

		if (g_ShaderHandle && g_FragHandle) glDetachShader(g_ShaderHandle, g_FragHandle);
		if (g_FragHandle) glDeleteShader(g_FragHandle);
		g_FragHandle = 0;

		if (g_ShaderHandle) glDeleteProgram(g_ShaderHandle);
		g_ShaderHandle = 0;

		if (g_FontTexture) {
			glDeleteTextures(1, &g_FontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			g_FontTexture = 0;
		}
	}

	static void ImGui_ImplGlfw_InstallCallbacks(GLFWwindow* window) {
		glfwSetCursorPosCallback(window, ImGui_ImplGlfw_CursorPosCallback);
		glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
		glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
		glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
		glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
	}

	bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks, const char* glsl_version) {
		g_Window = window;

		// Store GL version string so we can refer to it later in case we recreate shaders.
		if (glsl_version == NULL)
			glsl_version = "#version 150";
		IM_ASSERT((int) strlen(glsl_version) + 2 < IM_ARRAYSIZE(g_GlslVersion));
		strcpy(g_GlslVersion, glsl_version);
		strcat(g_GlslVersion, "\n");

		// Setup back-end capabilities flags
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;    // We can honor io.WantSetMousePos requests (optional, rarely used)

		// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
		io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
		io.ClipboardUserData = g_Window;


		// Load cursors
		// FIXME: GLFW doesn't expose suitable cursors for ResizeAll, ResizeNESW, ResizeNWSE. We revert to arrow cursor for those.
		g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

		if (install_callbacks)
			ImGui_ImplGlfw_InstallCallbacks(window);

		return true;
	}

	void ImGui_ImplGlfwGL3_Shutdown() {
		// Destroy GLFW mouse cursors
		for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
			glfwDestroyCursor(g_MouseCursors[cursor_n]);
		memset(g_MouseCursors, 0, sizeof(g_MouseCursors));

		// Destroy OpenGL objects
		ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
	}

	void ImGui_ImplGlfwGL3_NewFrame() {
		if (!g_FontTexture)
			ImGui_ImplGlfwGL3_CreateDeviceObjects();

		ImGuiIO& io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		int w, h;
		int display_w, display_h;
		glfwGetWindowSize(g_Window, &w, &h);
		glfwGetFramebufferSize(g_Window, &display_w, &display_h);
		io.DisplaySize = ImVec2((float) w, (float) h);
		io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float) display_w / w) : 0, h > 0 ? ((float) display_h / h) : 0);

		// Setup time step
		double current_time = glfwGetTime();
		io.DeltaTime = g_Time > 0.0 ? (float) (current_time - g_Time) : (float) (1.0f / 60.0f);
		g_Time = current_time;

		// Setup inputs
		// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
		if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED)) {
			// Set OS mouse position if requested (only used when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
			if (io.WantSetMousePos) {
				glfwSetCursorPos(g_Window, (double) io.MousePos.x, (double) io.MousePos.y);
			} else {
				double mouse_x, mouse_y;
				glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
				io.MousePos = ImVec2((float) mouse_x, (float) mouse_y);
			}
		} else {
			io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		}

		for (int i = 0; i < 3; i++) {
			// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
			io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
			g_MouseJustPressed[i] = false;
		}

		// Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0 &&
			glfwGetInputMode(g_Window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED) {
			ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
			if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None) {
				glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			} else {
				glfwSetCursor(g_Window,
							  g_MouseCursors[cursor] ? g_MouseCursors[cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
				glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
		}

		// Gamepad navigation mapping [BETA]
		memset(io.NavInputs, 0, sizeof(io.NavInputs));
		if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) {
			// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
			int axes_count = 0, buttons_count = 0;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
			const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
			MAP_BUTTON(ImGuiNavInput_Activate, 0);     // Cross / A
			MAP_BUTTON(ImGuiNavInput_Cancel, 1);     // Circle / B
			MAP_BUTTON(ImGuiNavInput_Menu, 2);     // Square / X
			MAP_BUTTON(ImGuiNavInput_Input, 3);     // Triangle / Y
			MAP_BUTTON(ImGuiNavInput_DpadLeft, 13);    // D-Pad Left
			MAP_BUTTON(ImGuiNavInput_DpadRight, 11);    // D-Pad Right
			MAP_BUTTON(ImGuiNavInput_DpadUp, 10);    // D-Pad Up
			MAP_BUTTON(ImGuiNavInput_DpadDown, 12);    // D-Pad Down
			MAP_BUTTON(ImGuiNavInput_FocusPrev, 4);     // L1 / LB
			MAP_BUTTON(ImGuiNavInput_FocusNext, 5);     // R1 / RB
			MAP_BUTTON(ImGuiNavInput_TweakSlow, 4);     // L1 / LB
			MAP_BUTTON(ImGuiNavInput_TweakFast, 5);     // R1 / RB
			MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
			MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
			MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
			MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG
			if (axes_count > 0 && buttons_count > 0)
				io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
			else
				io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
		}

		// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
		ImGui::NewFrame();
	}

	void imguiShutdown() {
		ImGui_ImplGlfwGL3_Shutdown();
		ImGui::DestroyContext();
	}

}
