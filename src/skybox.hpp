#pragma once
#include "entity.hpp"
#include "textureloader.hpp"


class Skybox : public Entity
{
public:
	bool init(OBJ::Data obj);
	void destroy();
	void update(float ms);
	void draw(glm::mat4 mvp)override;
	void set_cube_faces(const std::string relative_path);
	cube_textures get_cube_faces();

private:
	cube_textures faces;
};

