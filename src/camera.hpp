#pragma once

#include "common.hpp"

#include <vector>
#include <random>

// glm
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

// You might ask: Shouldn't this just work from cmath/math.h? That would make sense right?
// I have the same question. And yes I tried using the weird MS define thing, that did nothing.
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Camera {
public:
	float fieldOfView = 50.0f;
	float cameraMoveSpeed = 0.05f;
	float cameraRotateSpeed = 0.005f;
	float cameraZoomSpeed = 0.05f;

	glm::vec2 mouseScroll;
	glm::vec3 cameraDirection;
	glm::vec3 cameraPosition = { 0,10, -15 };
	glm::vec3 cameraAngle = { 0, 0, 0 };
	glm::vec3 cameraVerticalVector; // Note: This is what is typically reffered to as "up" for the viewer
	glm::vec3 cameraHorizontalVector;
	glm::mat4 m_view;

	bool move_forward, move_backward, move_left, move_right, rotate_left, rotate_right;

	void update(float ms);

	glm::mat4 getProjectionMatrix(float screen_x, float screen_y);

	glm::mat4 getViewMatrix();
};