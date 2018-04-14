//
// Created by eric on 4/4/18.
//

#pragma once

#include <GL/gl3w.h> //make sure this comes before any other opengl stuff, use for GLFWwindow, GLFWcursor
#include "GLFW/glfw3.h" //for GLuint
#include "imgui.h"

namespace Ui {
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// GLFW data
	static GLFWwindow*  g_Window = NULL;
	static double       g_Time = 0.0f;
	static bool         g_MouseJustPressed[3] = { false, false, false };
	static GLFWcursor*  g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

	// OpenGL3 data
	static char         g_GlslVersion[32] = "#version 150";
	static GLuint       g_FontTexture = 0;
	static GLuint       g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
	static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
	static GLint       g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
	static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

	//eric's stuff
	enum class SpawnWindowState{
		SPAWN_SELECTOR,
		SPAWN_DEFENSIVE_BUILDINGS,
		SPAWN_ECONOMIC_BUILDINGS
	};

	static SpawnWindowState spawnWindowState = SpawnWindowState::SPAWN_SELECTOR;
	static int uiHeight = 200;
	static int spawnWindowWidth = 300;

	static ImVec2 topLeft;
	static ImVec2 bottomRight;
	static ImVec2 unitSelectionSize;

	// UI logos and textures
	static ImTextureID gameLogo;
	static ImVec2 gameLogoSize;

	//funcs
	bool createWindow(); //does glfw stuff

	GLFWwindow* getWindow();

	void imguiSetup(); //load imgui bits

	void imguiDrawLaunchMenu(); //game launch menu

	void imguiDrawPauseMenu(); //game launch menu

	void imguiGenerateScreenObjects(); //main ui

	void imguiShutdown(); //cleanup

	ImVec2 getSelectionBoxStartPos(const ImVec2& startClickPos, const ImVec2& endClickPos);

	bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks, const char* glsl_version);

	void ImGui_ImplGlfw_CursorPosCallback(GLFWwindow *window, double xpos, double ypos);

	void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);

	void ImGui_ImplGlfw_ScrollCallback(GLFWwindow*, double xoffset, double yoffset);

	void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void ImGui_ImplGlfw_CharCallback(GLFWwindow*, unsigned int c);

	void ImGui_ImplGlfwGL3_RenderDrawData(ImDrawData* draw_data);

	bool ImGui_ImplGlfwGL3_CreateFontsTexture();

	bool ImGui_ImplGlfwGL3_CreateDeviceObjects();

	void ImGui_ImplGlfwGL3_NewFrame();

}