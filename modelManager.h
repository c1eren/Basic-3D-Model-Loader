#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class ModelManager {
public:
	int getMouseButtonState() const { return m_mouseButtonState; }
	void setMouseButtonState(int mBS) { m_mouseButtonState = mBS; }

	glm::mat4 getModelMatrix() const { return m_modelMatrix; }
	void setModelMatrix(glm::mat4 mMX) { m_modelMatrix = mMX; }

	glm::mat3 getNormalMatrix() const { return m_normalMatrix; }
	void setNormallMatrix(glm::mat3 nMX) { m_normalMatrix = nMX; }

private:
	int m_mouseButtonState = GLFW_RELEASE;
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat3 m_normalMatrix = glm::mat3(1.0f);
};