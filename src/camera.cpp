#include "camera.hpp"
#include <iostream>
#include <algorithm> 

void Camera::update(float ms)
{
	cameraDirection = glm::vec3(
		cos(cameraAngle.y)*sin(cameraAngle.x),
		sin(cameraAngle.y),
		cos(cameraAngle.y)*cos(cameraAngle.x)
	);
	cameraHorizontalVector = glm::vec3(sin(cameraAngle.x - M_PI / 2.0f), 0, cos(cameraAngle.x - M_PI / 2.0f));
	glm::vec3 cameraVector2 = glm::vec3(sin(cameraAngle.x), 0, cos(cameraAngle.x));
	cameraVerticalVector = glm::cross(cameraHorizontalVector, cameraDirection);

	if (!z_held) {
		if (mouseScroll.y > 0) {
			cameraAngle.y = std::min((float)M_PI / 2, cameraAngle.y + mouseScroll.y * cameraZoomSpeed);
		}
		if (mouseScroll.y < 0) {
			cameraAngle.y = std::max((float)-M_PI, cameraAngle.y + mouseScroll.y * cameraZoomSpeed);
		}
	} else {
		cameraPosition += mouseScroll.y * cameraZoomSpeed * cameraDirection;
	}

	mouseScroll = { 0, 0 }; // Scroll needs to be reset, it won't do that on it's own

	if (move_forward)cameraPosition += cameraVector2 * ms * cameraMoveSpeed;
	if (move_backward)cameraPosition -= cameraVector2 * ms * cameraMoveSpeed;

	if (move_right) cameraPosition += cameraHorizontalVector * ms * cameraMoveSpeed;
	if (move_left) cameraPosition -= cameraHorizontalVector * ms * cameraMoveSpeed;

	if (rotate_right)cameraAngle.x += cameraRotateSpeed * ms;
	if (rotate_left)cameraAngle.x -= cameraRotateSpeed * ms;
}

glm::mat4 Camera::getProjectionMatrix(float screen_x, float screen_y)
{
	return glm::perspective(glm::radians(fieldOfView), screen_x / screen_y, 0.1f, 750.0f);
}

glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraDirection, cameraVerticalVector);
}
