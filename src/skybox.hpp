#pragma once
#include "entity.hpp"
#include "stb_image.h"

struct cube_textures {
	std::string front;
	std::string back;
	std::string top;
	std::string bottom;
	std::string left;
	std::string right;
};


class Skybox : public Entity
{
public:
	bool init(OBJ::Data obj);

	void destroy();

	void update(float ms);

	void draw(glm::mat4 mvp)override;

	void set_cube_faces(const std::string path);

	cube_textures get_cube_faces();

	bool load_side_texture(GLuint texture, GLenum side, const std::string texture_image);

	void generate_cube_map(const cube_textures &faces, GLuint *cube_texture);

private:
	cube_textures faces;
};

