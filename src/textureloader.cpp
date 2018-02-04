#include "textureloader.hpp"
#include "common.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image/stb_image.h"

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