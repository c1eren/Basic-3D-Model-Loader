#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct TexturesBound {
	unsigned int diff = 0;
	unsigned int spec = 0;
	unsigned int norm = 0;
};

class ModelManager {
public:
	bool getFirstDraw() const { return m_firstDraw; }
	void setFirstDraw(bool fD) { m_firstDraw = fD; }

	bool getRotationOn() const { return m_rotationOn; }
	void setRotationOn(bool rO) { m_rotationOn = rO; }

	bool getRebindRequired() const { return m_rebindRequired; }
	void setRebindRequired(bool rR) { m_rebindRequired = rR; }

	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 mP) { m_position = mP;}// setModelMatrix(glm::translate(glm::mat4(1.0f), mP)); }

	glm::mat4 getModelMatrix() const { return m_modelMatrix; }
	void setModelMatrix(glm::mat4 mMX) { m_modelMatrix = mMX; }

	glm::mat3 getNormalMatrix() const { return m_normalMatrix; }
	void setNormalMatrix(glm::mat3 nMX) { m_normalMatrix = nMX; }

	TexturesBound getTexturesBound() const { return m_tBound; }
	void setTexturesBound(TexturesBound tB) { m_tBound = tB; }

	MaterialProperties getMatPropSet() const { return m_matPropSet; }
	void setMatPropSet(MaterialProperties mPS) { m_matPropSet = mPS; }

	MaterialColors getMatColSet() const { return m_matColSet; }
	void setMatColSet(MaterialColors mCS) { m_matColSet = mCS; }

	bool getHasMoved() const { return m_hasMoved; }
	void setHasMoved(bool hM) { m_hasMoved = hM; }

public:
	static unsigned int m_id;
	unsigned int id = 0;

private:
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat3 m_normalMatrix = glm::mat3(1.0f);
	static TexturesBound m_tBound; // Static for now for simplicity 
	MaterialProperties m_matPropSet;
	MaterialColors m_matColSet;

	// Properties
	bool m_firstDraw = 1;
	bool m_rebindRequired = 0;
	bool m_hasMoved = 1;
	bool m_rotationOn = 0;
	glm::vec3 m_position = glm::vec3(0.0f);
};