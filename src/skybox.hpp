#pragma once
#include "entity.hpp"
#include "stb_image.h"

struct cube_faces {
	char* front;
	char* back;
	char* top;
	char* bottom;
	char* left;
	char* right;
};


class Skybox : public Entity
{
public:
	bool init(OBJ::Data obj);

	void destroy();

	void update(float ms);

	void draw(glm::mat4 mvp)override;

	bool load_side_texture(GLuint texture, GLenum side, const char *texture_image);

	void generate_cube_map(const cube_faces* faces, GLuint *cube_texture);

private:
		cube_faces faces;
};

