// Header
#include "unitmanager.hpp"
#include "unitcomp.hpp"
#include "logger.hpp"
#include "world.hpp"
#include "collisiondetection.hpp"
#include "particle.hpp"
#include "aimanager.hpp"
#include "unit.hpp"
#include "attackManger.hpp"
#include "buildingmanager.hpp"

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

#include "imgui.h"
#include "GLFW/glfw3.h"
#include <GL/gl3w.h>
/*! /brief Boilerplate function for OpenGL 2.0 rendering.
 *
 *  This function isn't written by us, but is mandatory
 *  boilerplate from the library. It can be copy/pasted
 *  into your projects, but should really be part of the
 *  library itself?
 */


bool show_demo_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static GLFWwindow*  g_Window = NULL;
static double       g_Time = 0.0f;
static bool         g_MouseJustPressed[3] = { false, false, false };
static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

static char         g_GlslVersion[32] = "#version 150";
static GLuint       g_FontTexture = 0;
static int          g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

bool  ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks, const char* glsl_version);

void World::imguiSetup()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfwGL3_Init(m_window, true,g_GlslVersion);

	ImGui::StyleColorsDark();

}

void ImGui_ImplGlfw_MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/)
{
	if (action == GLFW_PRESS && button >= 0 && button < 3)
		g_MouseJustPressed[button] = true;
}

void ImGui_ImplGlfw_ScrollCallback(GLFWwindow*, double xoffset, double yoffset)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheelH += (float)xoffset;
	io.MouseWheel += (float)yoffset;
}

void ImGui_ImplGlfw_KeyCallback(GLFWwindow*, int key, int, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (action == GLFW_PRESS)
		io.KeysDown[key] = true;
	if (action == GLFW_RELEASE)
		io.KeysDown[key] = false;

	(void)mods; // Modifiers are not reliable across systems
	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void ImGui_ImplGlfw_CharCallback(GLFWwindow*, unsigned int c)
{
	ImGuiIO& io = ImGui::GetIO();
	if (c > 0 && c < 0x10000)
		io.AddInputCharacter((unsigned short)c);
}
bool  ImGui_ImplGlfwGL3_Init(GLFWwindow* window, bool install_callbacks, const char* glsl_version)
{
	g_Window = window;

	// Store GL version string so we can refer to it later in case we recreate shaders.
	if (glsl_version == NULL)
		glsl_version = "#version 150";
	IM_ASSERT((int)strlen(glsl_version) + 2 < IM_ARRAYSIZE(g_GlslVersion));
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

//	io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
//	io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
	io.ClipboardUserData = g_Window;
#ifdef _WIN32
	io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

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
	{
		glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
		glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
		glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
		glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
	}


	return true;
}



void ImGui_ImplGlfwGL3_RenderDrawData(ImDrawData* draw_data)
{
	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
//	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
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
	glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	const float ortho_projection[4][4] =
			{
					{ 2.0f/io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
					{ 0.0f,                  2.0f/-io.DisplaySize.y, 0.0f, 0.0f },
					{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
					{-1.0f,                  1.0f,                   0.0f, 1.0f },
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
	glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

	// Draw
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
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
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool ImGui_ImplGlfwGL3_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

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
	io.Fonts->TexID = (void *)(intptr_t)g_FontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

bool ImGui_ImplGlfwGL3_CreateDeviceObjects()
{
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

	const GLchar* vertex_shader_with_version[2] = { g_GlslVersion, vertex_shader };
	const GLchar* fragment_shader_with_version[2] = { g_GlslVersion, fragment_shader };

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


void ImGui_ImplGlfwGL3_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplGlfwGL3_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h;
	int display_w, display_h;
	glfwGetWindowSize(g_Window, &w, &h);
	glfwGetFramebufferSize(g_Window, &display_w, &display_h);
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	double current_time =  glfwGetTime();
	io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
	g_Time = current_time;

	// Setup inputs
	// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
	if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED))
	{
		// Set OS mouse position if requested (only used when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
		if (io.WantSetMousePos)
		{
			glfwSetCursorPos(g_Window, (double)io.MousePos.x, (double)io.MousePos.y);
		}
		else
		{
			double mouse_x, mouse_y;
			glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
			io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
		}
	}
	else
	{
		io.MousePos = ImVec2(-FLT_MAX,-FLT_MAX);
	}

	for (int i = 0; i < 3; i++)
	{
		// If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
		g_MouseJustPressed[i] = false;
	}

	// Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
	if ((io.ConfigFlags & ImGuiConfigFlags_NoSetMouseCursor) == 0 && glfwGetInputMode(g_Window, GLFW_CURSOR) != GLFW_CURSOR_DISABLED)
	{
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
		{
			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			glfwSetCursor(g_Window, g_MouseCursors[cursor] ? g_MouseCursors[cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	// Gamepad navigation mapping [BETA]
	memset(io.NavInputs, 0, sizeof(io.NavInputs));
	if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)
	{
		// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)       { if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) { float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; v = (v - V0) / (V1 - V0); if (v > 1.0f) v = 1.0f; if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
		int axes_count = 0, buttons_count = 0;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
		MAP_BUTTON(ImGuiNavInput_Activate,   0);     // Cross / A
		MAP_BUTTON(ImGuiNavInput_Cancel,     1);     // Circle / B
		MAP_BUTTON(ImGuiNavInput_Menu,       2);     // Square / X
		MAP_BUTTON(ImGuiNavInput_Input,      3);     // Triangle / Y
		MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);    // D-Pad Left
		MAP_BUTTON(ImGuiNavInput_DpadRight,  11);    // D-Pad Right
		MAP_BUTTON(ImGuiNavInput_DpadUp,     10);    // D-Pad Up
		MAP_BUTTON(ImGuiNavInput_DpadDown,   12);    // D-Pad Down
		MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);     // L1 / LB
		MAP_BUTTON(ImGuiNavInput_FocusNext,  5);     // R1 / RB
		MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);     // L1 / LB
		MAP_BUTTON(ImGuiNavInput_TweakFast,  5);     // R1 / RB
		MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
		MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
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


void World::imguiRenderThings() {
	glfwPollEvents();
	ImGui_ImplGlfwGL3_NewFrame();

	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	{
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (show_demo_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// Rendering
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_window);
}


// World initialization
bool World::init() {
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
	m_window = glfwCreateWindow(Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOW_HEIGHT, Config::WINDOW_TITLE,
								nullptr, nullptr);
	m_screen = glm::vec2(Config::INITIAL_WINDOW_WIDTH, Config::INITIAL_WINDOW_HEIGHT);
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
	auto mouse_button_redirect = [](GLFWwindow* wnd, int _0, int _1, int _2) {
		((World*) glfwGetWindowUserPointer(wnd))->on_mouse_button(wnd, _0, _1, _2);
	};
	auto window_size_callback = [](GLFWwindow* window, int width, int height) {
		Global::windowWidth = static_cast<size_t>(width);
		Global::windowHeight = static_cast<size_t>(height);
	};
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);
	glfwSetScrollCallback(m_window, scroll_offset_redirect);
	glfwSetMouseButtonCallback(m_window, mouse_button_redirect);
	glfwSetWindowSizeCallback(m_window, window_size_callback);

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


	if (!initMeshTypes(Model::meshSources)) {
		logger(LogLevel::ERR) << "Failed to initialize renderers \n";
	}

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
	auto temp1 = Unit::spawn(Unit::UnitType::SPHERICAL_DEATH, {startx, 0, startz}, GamePieceOwner::PLAYER);
	temp1->moveTo(UnitState::MOVE, targetx, targetz);

	startx = 39, startz = 19;
	auto temp2 = Unit::spawn(Unit::UnitType::TANK, {startx, 0, startz}, GamePieceOwner::AI);
//	temp2->moveTo(targetx, targetz);

	startx = 39, startz = 1;
	auto temp3 = Unit::spawn(Unit::UnitType::SPHERICAL_DEATH, {startx, 0, startz}, GamePieceOwner::PLAYER);
	temp3->moveTo(UnitState::MOVE, targetx, targetz);

	startx = 20, startz = 20;
	auto temp4 = Unit::spawn(Unit::UnitType::TANK, {startx, 0, startz}, GamePieceOwner::AI);

	// Example use of targeting units.
	AttackManager::registerTargetUnit(temp2, temp1);

	selectedTileCoordinates.rowCoord = Global::levelWidth / 2;
	selectedTileCoordinates.colCoord = Global::levelHeight / 2;

	// Unit test stuff

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
//	AttackManager::update(elapsed_ms);
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

	for (auto renderer : Model::meshRenderers) {
		renderer->render(projectionView);
	}


	// make skybox rotate by 0.001 * pi/4 radians around y axis, every frame
//	float y_rotation = 0.005 * glm::quarter_pi<float>();
//	m_skybox.setRotation(glm::vec3(0.0, y_rotation, 0.0));
//	m_skybox.applyTransformations();
	m_skybox.getCameraPosition(camera.position);
	m_skybox.draw(projection * view * m_skybox.getModelMatrix());

	for (const auto& emitter : level.emitters) {
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
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		escapePressed = true;
	}

	// File saving
	if (action == GLFW_RELEASE && key == GLFW_KEY_P) {
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

//returns w x h
std::pair<int, int> World::getWindowSize() {
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
	return {windowWidth, windowHeight};
}

void World::on_mouse_button(GLFWwindow* window, int button, int action, int mods) {
	glm::vec3 coords = {selectedTileCoordinates.colCoord, 0, selectedTileCoordinates.rowCoord};
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (coords.x < 0 || coords.x + 1 > Global::levelWidth)
			return;
		if (coords.z < 0 || coords.z + 1 > Global::levelHeight)
			return;
		level.placeTile(Model::MeshType::GUN_TURRET, coords);
		logger(LogLevel::INFO) << "Right click detected " << '\n';
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (m_dist(m_rng) < 0.5) {
			level.placeEntity(Model::MeshType::FRIENDLY_RANGED_UNIT, coords, GamePieceOwner::PLAYER);
		} else {
			level.placeEntity(Model::MeshType::FRIENDLY_FIRE_UNIT, coords, GamePieceOwner::PLAYER);
		}
		logger(LogLevel::INFO) << "Left click detected " << '\n';
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		for (const auto& entity : Global::playerUnits) {
			entity->rigidBody.setVelocity((coords - entity->rigidBody.getPosition()) / 5000.0f);
		}
	}
}

