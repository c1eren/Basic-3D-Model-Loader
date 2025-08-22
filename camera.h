#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "globalVariables.h"

enum class Direction : unsigned char {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Defaults
const float yaw = -90.0f;
const float pitch = 0.0f;
const float speed = 200.0f;
const float sensitivity = 0.1f;
const float zoom = 45.0f;
const float maxSpeed = 200.0f;
const float accelRate = 10.0f;

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 defaultPos(0.0f, 0.0f, 5.0f);
const glm::vec3 defaultFront(0.0f, 0.0f, -1.0f);
const glm::vec3 defaultXY(0.0f);

const unsigned int centerScreenX = (viewport_width / 2);
const unsigned int centerScreenY = (viewport_height / 2);

class Camera {
private:
	glm::vec3 cameraPos = defaultPos;
	glm::vec3 cameraFront = defaultFront;
	glm::vec3 cameraX = defaultXY;
	glm::vec3 cameraY = defaultXY;

	float camZoom;
	float lastX = static_cast<float>(centerScreenX);
	float lastY = static_cast<float>(centerScreenY);
	float yaw = yaw;
	float pitch = pitch;


	bool hasMoved = 1;

public:
	Camera();
	~Camera() {}
	
	// Update position vector
	void moveForward(const float speed);
	void moveBackward(const float speed);
	void moveLeft(const float speed);
	void moveRight(const float speed);
	void moveUp(const float speed);
	void moveDown(const float speed);

	// Update Euler values
	void rotateYaw(const float speed);
	void rotatePitch(const float speed); // Can look at storing speed in camera 

};

	void updateEulerValues(float xoffset, float yoffset);

	void processKeyPress(Direction dir);

	void processMouseScroll(float yoffset);

	void updateCameraFrontVectors();

	glm::mat4 getViewMatrix();

	glm::mat4 lookAT();

	void setLastXY(float x, float y) { lastX = x; lastY = y; }

};




