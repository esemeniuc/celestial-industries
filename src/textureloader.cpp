#include "textureloader.hpp"
#include "common.hpp"

// apparently this define for STB library needs to be inside a cpp file, not an hpp
#define STB_IMAGE_IMPLEMENTATION
//ignore warnings from stb_image
#pragma GCC diagnostic push // save diagnostic state
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#include "../ext/stb_image/stb_image.h"
#pragma GCC diagnostic pop

Texture::Texture()
{

}

Texture::~Texture()
{
	if (id != 0) glDeleteTextures(1, &id);
}

bool Texture::load_from_file(const char* path)
{
	if (path == nullptr)
		return false;

	stbi_uc* data = stbi_load(path, &width, &height, NULL, 4);
	if (data == NULL)
		return false;

	gl_flush_errors();
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);
	return !gl_has_errors();
}

bool Texture::is_valid()const
{
	return id != 0;
}

bool Texture::load_side_texture(GLuint texture, GLenum side, const std::string& texture_image_path)
{
	// load image data from file
	if (texture_image_path.empty()) return false;
	int textureWidth, textureHeight, num_of_channels = 4;
	stbi_uc* texture_data = stbi_load(texture_image_path.c_str(), &textureWidth, &textureHeight, nullptr, num_of_channels);
	if (texture_data == nullptr) return false;

	// bind texture to target
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	// assign image data to a target texture
	gl_flush_errors();
	glTexImage2D(side, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

	// free memory
	free(texture_data);
	return !gl_has_errors();
}

void Texture::generate_cube_map(const cube_textures &faces, GLuint *cube_texture)
{
	// assign texture to Texture unit 0
	glActiveTexture(GL_TEXTURE0);
	// generate texture name
	glGenTextures(1, cube_texture);

	// load textures for each side of the skybox cube
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, faces.front);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, faces.back);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, faces.top);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, faces.bottom);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, faces.left);
	load_side_texture(*cube_texture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, faces.right);

	// cube map texture formatting (bilinear filtering, clamp to edge)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}