#pragma once

#include "entity.hpp"
#include "textureloader.hpp"


class Skybox : public OBJRenderable {
public:
	bool init(OBJ::Data obj);
	void destroy();
	void update(float ms);
	void draw(glm::mat4 mvp) override;
	void set_cube_faces(const std::string relative_path);
	glm::mat4 getModelMatrix();
	void setCameraPosition(glm::vec3 pos);

	cube_textures get_cube_faces();

private:
	glm::vec3 cameraPosition;
	cube_textures faces;
};

