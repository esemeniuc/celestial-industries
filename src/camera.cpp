#include <algorithm>
#include "camera.hpp"
#include "global.hpp"


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
	position += horizontalVector * ms * deltaX * mousePanSensitivity;
	//pan up down update
	position += cameraVector2 * ms * deltaZ * mousePanSensitivity;

}

glm::mat4 Camera::getProjectionMatrix(float screen_x, float screen_y) {
	return glm::perspective(glm::radians(fieldOfView), screen_x / screen_y, 0.1f, 750.0f);
}

glm::mat4 Camera::getViewMatrix() {
	return glm::lookAt(position, position + direction, verticalVector);
}

//delta get set to 0 on every update() call
void Camera::pan(double xpos, double ypos) {
	int leftBound = panDetectionWidth;
	int rightBound = (int) Global::windowWidth - panDetectionWidth;
	int topBound = panDetectionWidth; //top border
	int botBound = (int) Global::windowHeight - panDetectionWidth; //bottom border
	if (xpos <= leftBound) { //handles NW, SW, and W cases
		if (ypos <= topBound) {
			deltaZ = 1;
		} else if (ypos >= botBound) {
			deltaZ = -1;
		} else {
			deltaZ = 0;
		}
		deltaX = -1;
	} else if (xpos >= rightBound) { //handles NE, SE, and E cases
		if (ypos <= topBound) {
			deltaZ = 1;
		} else if (ypos >= botBound) {
			deltaZ = -1;
		} else {
			deltaZ = 0;
		}
		deltaX = 1;
	} else if (ypos <= topBound) { //N case
		deltaZ = 1;
		deltaX = 0;
	} else if (ypos >= botBound) { //S case
		deltaZ = -1;
		deltaX = 0;
	} else { //not along edges
		deltaX = 0;
		deltaZ = 0;
	}
}
