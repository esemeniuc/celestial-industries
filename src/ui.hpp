//
// Created by eric on 4/4/18.
//

#pragma once

#include <GL/gl3w.h> //make sure this comes before any other opengl stuff
#include "GLFW/glfw3.h"
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
	static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
	static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
	static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
	static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

	//eric's stuff
	enum class SpawnWindowState{
		SPAWN_SELECTOR,
		SPAWN_DEFENSIVE_BUILDINGS,
		SPAWN_ECONOMIC_BUILDINGS
	};

	static SpawnWindowState spawnWindowState = SpawnWindowState::SPAWN_SELECTOR;
	static bool cursorNormalMode = true;
	static int uiHeight = 200;

	static int spawnWindowWidth = 300;

	static int resourceCounterWidth = 230;
	static int resourceCounterHeight = 70;
	static int resourceCounterHeightOffset = 10; //starting position offset from top
	static int resourceCounterWidthOffset = resourceCounterWidth + 10; //starting position offset from right side of screen

	//funcs
	void imguiSetup(GLFWwindow* window);

	void imguiGenerateScreenObjects();

	bool ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks, const char* glsl_version);

	void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/);

	void ImGui_ImplGlfw_ScrollCallback(GLFWwindow*, double xoffset, double yoffset);

	void ImGui_ImplGlfw_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void ImGui_ImplGlfw_CharCallback(GLFWwindow*, unsigned int c);

	void ImGui_ImplGlfwGL3_RenderDrawData(ImDrawData* draw_data);

	bool ImGui_ImplGlfwGL3_CreateFontsTexture();

	bool ImGui_ImplGlfwGL3_CreateDeviceObjects();

	void ImGui_ImplGlfwGL3_NewFrame();
}