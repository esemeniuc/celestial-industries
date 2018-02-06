#pragma once
#define NOMINMAX

#include <string>
#include <gl3w.h>
#include <GLFW/glfw3.h>

struct cube_textures {
	std::string front;
	std::string back;
	std::string top;
	std::string bottom;
	std::string left;
	std::string right;
};

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
	bool load_side_texture (GLuint texture, GLenum side, const std::string texture_image_path);
	void generate_cube_map (const cube_textures & faces, GLuint * cube_texture);	
};