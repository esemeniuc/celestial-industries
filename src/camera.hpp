#pragma once

#include <random>
#include <vector>
#include "common.hpp"

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

#ifndef M_PI
#define M_PI glm::pi<float>()
#endif

class Camera {
public:
	const float fieldOfView = 50.0f;
	const float moveSpeed = 0.05f;
	const float rotateSpeed = 0.005f;
	const float zoomSpeed = 1.0f;
	const float mousePanSensitivity = 0.001f;

	glm::vec2 mouseScroll;
	glm::vec3 direction;
	glm::vec3 position; //gets overwritten in world.cpp
	glm::vec3 angle = {0, -M_PI / 4, 0}; //pi/4 for isomorphic view
	glm::vec3 verticalVector; // Note: This is what is typically referred to as "up" for the viewer
	glm::vec3 horizontalVector;
	glm::mat4 view;

	bool move_forward, move_backward, move_left, move_right, rotate_left, rotate_right, z_held;

	void update(float ms);

	void moveUpDown(int y);

	void moveLeftRight(int x);

	glm::mat4 getProjectionMatrix(float screen_x, float screen_y);

	glm::mat4 getViewMatrix();

private:
	const int deltaRatioLimit = 8; //prevents jerkiness
	const int deltaXLimit = Config::WINDOW_WIDTH / deltaRatioLimit;
	const int deltaYLimit = Config::WINDOW_HEIGHT / deltaRatioLimit;
	int deltaX = 0, deltaY = 0;
};