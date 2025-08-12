#pragma once

#include <glm/glm.hpp>


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

class CameraManager {

};


