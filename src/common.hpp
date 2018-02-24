#pragma once
#include "config.hpp"
#include "logger.hpp"
#include "objloader.hpp"

// stlib
#include <fstream> // stdout, stderr..
#include <vector>
#include <string>
#include <sstream>

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
#ifdef _MSC_VER
  #define data_path "../data"
  #define shader_path(name) "../shaders/" name
#else
  #define data_path "data"
  #define shader_path(name) "shaders/" name
#endif
#define textures_path(name) data_path "/textures/" name
#define audio_path(name) data_path "/audio/" name
#define mesh_path(name) data_path "/meshes/" name


// Our stuff
extern Logger logger; //use extern so we share the logger in main.cpp with all other cpp files

std::vector<std::string> splitString(std::string input, const char seperator);
char separator();

std::string pathBuilder(std::vector<std::string> parts);
std::string pathAppender(std::string base, std::vector<std::string> parts);

// Not much math is needed and there are already way too many libraries linked (:    <--- That's not just evil, it's also silly
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3  mul(const mat3& l, const mat3& r);
vec2  normalize(vec2 v);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and 
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
	GLuint numIndices;
	OBJ::Material material;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char* vs_path, const char* fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{
	std::vector<Mesh> meshes;
	Effect effect;
	glm::mat4 model = glm::mat4(1.0f); // Identity matrix

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(glm::mat4 viewProjection) = 0;
};

template<class T>
T clamp(T value, T lower, T upper) {
	if (value > upper) {
		value = upper;
	}

	if (value < lower) {
		value = lower;
	}
	return value;
}