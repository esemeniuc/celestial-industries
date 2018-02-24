#include <algorithm>
#include "camera.hpp"


void Camera::update(float ms) {
//	logger(LogLevel::DEBUG) <<
//							"Cam pos at x: " << position.x <<
//							", y: " << position.y <<
//							", z: " << position.z << '\n';

	direction = glm::vec3(
			cos(angle.y) * sin(angle.x),
			sin(angle.y),
			cos(angle.y) * cos(angle.x)
	);
	horizontalVector = glm::vec3(sin(angle.x - M_PI / 2.0f), 0, cos(angle.x - M_PI / 2.0f));
	glm::vec3 cameraVector2 = glm::vec3(sin(angle.x), 0, cos(angle.x));
	verticalVector = glm::cross(horizontalVector, direction);

	if (!z_held) {
		if (mouseScroll.y > 0) {
			angle.y = std::min((float) M_PI / 2, angle.y + mouseScroll.y * zoomSpeed / 15.0f);
		}
		if (mouseScroll.y < 0) {
			angle.y = std::max((float) -M_PI, angle.y + mouseScroll.y * zoomSpeed / 15.0f);
		}
	} else {
		position += mouseScroll.y * zoomSpeed * direction;
	}

	mouseScroll = {0, 0}; // Scroll needs to be reset, it won't do that on it's own

	if (move_forward)position += cameraVector2 * ms * moveSpeed;
	if (move_backward)position -= cameraVector2 * ms * moveSpeed;

	if (move_right) position += horizontalVector * ms * moveSpeed;
	if (move_left) position -= horizontalVector * ms * moveSpeed;

	if (rotate_left)angle.x += rotateSpeed * ms;
	if (rotate_right)angle.x -= rotateSpeed * ms;

	//pan left right update
	position += horizontalVector * ms * (float) deltaX * mousePanSensitivity;
	//pan up down update
	position += cameraVector2 * ms * (float) deltaY * mousePanSensitivity;

	//reset for the next move so we don't get camera drift
	deltaX = 0;
	deltaY = 0;
}

glm::mat4 Camera::getProjectionMatrix(float screen_x, float screen_y) {
	return glm::perspective(glm::radians(fieldOfView), screen_x / screen_y, 0.1f, 750.0f);
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + direction, verticalVector);
}

void Camera::moveLeftRight(int x) {
	deltaX = clamp(x, -deltaXLimit, deltaXLimit);
}

void Camera::moveUpDown(int y) {
	deltaY = clamp(y, -deltaYLimit, deltaYLimit);
}