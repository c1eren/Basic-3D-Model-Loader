#pragma once

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "cameraManager.h"


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




