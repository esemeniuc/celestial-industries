#pragma once

#include "entity.hpp"
#include "textureloader.hpp"


class Skybox {
public:
	bool init(OBJ::Data obj);
	void destroy();
	void update(float ms);
	void draw(glm::mat4 mvp);
	void set_cube_faces(const std::string relative_path);
	glm::mat4 getModelMatrix();
	void setCameraPosition(glm::vec3 pos);

	cube_textures get_cube_faces();
    std::shared_ptr<Shader> shader;

private:
	glm::vec3 cameraPosition;
	cube_textures faces;
    std::shared_ptr<std::vector<Mesh>> meshes;
    glm::mat4 model = glm::mat4(1.0f);
};

