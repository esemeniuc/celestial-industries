#pragma once
#include "config.hpp"
#include "logger.hpp"
#include "objloader.hpp"

// stlib
#include <fstream> // stdout, stderr..
#include <vector>
#include <string>
#include <sstream>
#include <memory>

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

// Because iterating over pointer is good
template<typename T>
auto inline begin(std::shared_ptr<T> ptr) -> typename T::iterator {
    return ptr->begin();
}
template<typename T>
auto inline end(std::shared_ptr<T> ptr) -> typename T::iterator {
    return ptr->end();
}

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

struct Coord {
	int rowCoord, colCoord;

	Coord() = default;

	Coord(int _rowCoord, int _colCoord) : rowCoord(_rowCoord), colCoord(_colCoord) {}
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

std::pair<bool, std::shared_ptr<std::vector<Mesh>>> objToMesh(OBJ::Data obj);
