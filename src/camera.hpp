#pragma once

#include "common.hpp"
#include <vector>
#include <random>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#ifndef M_PI
#define M_PI glm::pi<float>()
#endif

class Camera {
public:
	float fieldOfView = 50.0f;
	float moveSpeed = 0.05f;
	float rotateSpeed = 0.005f;
	float zoomSpeed = 1.0f;

	glm::vec2 mouseScroll;
	glm::vec3 direction;
	glm::vec3 position = { 0,10, -15 };
	glm::vec3 angle = { 0, 0, 0 };
	glm::vec3 verticalVector; // Note: This is what is typically reffered to as "up" for the viewer
	glm::vec3 horizontalVector;
	glm::mat4 view;

	bool move_forward, move_backward, move_left, move_right, rotate_left, rotate_right, z_held;

	void update(float ms);

	glm::mat4 getProjectionMatrix(float screen_x, float screen_y);

	glm::mat4 getViewMatrix();
};