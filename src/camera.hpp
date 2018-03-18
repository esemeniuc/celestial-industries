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
	const int panDetectionWidth = 40; //in pixels
	const float mousePanSensitivity = 0.01f;

	glm::vec2 mouseScroll;
	glm::vec3 direction;
	glm::vec3 position; //gets overwritten in world.cpp
	glm::vec3 angle = {M_PI, -M_PI / 4, 0}; //pi/4 for isomorphic view
	glm::vec3 verticalVector; // Note: This is what is typically referred to as "up" for the viewer
	glm::vec3 horizontalVector;
	glm::mat4 view;

	bool move_forward, move_backward, move_left, move_right, rotate_left, rotate_right, z_held;

	int deltaX = 0;
	int deltaY = 0;

	void update(float ms);

	void pan(int x, int y);

	glm::mat4 getProjectionMatrix(float screen_x, float screen_y);

	glm::mat4 getViewMatrix();

private:
};