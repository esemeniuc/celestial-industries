#pragma once

#include "common.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// The base class for every tile in the game
class Tile : public Renderable
{
	// shared stuff will go here at some point

public:
	bool init(OBJ::Data obj);

	void destroy();

	void update(float ms);

	void draw(glm::mat4 mvp)override;

private:
	// TODO: Remove the ones we don't use... so all of them?
	glm::vec2 m_position;
	glm::vec3 m_scale;
	float m_z_rotation;
	size_t m_indices;
};