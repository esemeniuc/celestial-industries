#pragma once

#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// Texture wrapper. Yes I lifted it from common. I did it to avoid a cyclical dependency, ok?
struct Texture
{
	Texture();
	~Texture();
	Texture(Texture const&) = delete;

	GLuint id = 0;
	int width;
	int height;

	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	bool is_valid()const; // True if texture is valid
};