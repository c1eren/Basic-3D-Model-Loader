#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f); // World up position (absolute, normalized)

class Camera {
public:
	float camYaw;
	float camPitch;

	float lastX = 400;
	float lastY = 300;

	bool firstMouse = 1;

	float camZoom;
	float camSensitivity;
	float camSpeed;

	glm::vec3 cameraPos;	// Camera world position
	glm::vec3 cameraFront;	// CameraTarget (what's right in front of us)
	glm::vec3 cameraX;	
	glm::vec3 cameraY;

	bool hasMoved = 1;
	bool flashLightOn = 0;


	//glm::mat4 camView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraY); // lookAt what's in front of us


	Camera(glm::vec3 camPosition = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f));

	void updateEulerValues(float xoffset, float yoffset);

	void processKeyPress(Direction dir);

	void processMouseScroll(float yoffset);

	void updateCameraFrontVectors();

	glm::mat4 getViewMatrix();

	glm::mat4 lookAT();

	void setLastXY(float x, float y) { lastX = x; lastY = y; }

};




