#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct TexturesBound {
	unsigned int diff = 0;
	unsigned int spec = 0;
	unsigned int norm = 0;
};

class ModelManager {
public:
	bool getRotationOn() const { return m_RotationOn; }
	void setRotationOn(bool rO) {m_RotationOn = rO; }

	glm::mat4 getModelMatrix() const { return m_modelMatrix; }
	void setModelMatrix(glm::mat4 mMX) { m_modelMatrix = mMX; }

	glm::mat3 getNormalMatrix() const { return m_normalMatrix; }
	void setNormallMatrix(glm::mat3 nMX) { m_normalMatrix = nMX; }

	TexturesBound getTexturesBound() const { return m_tBound; }
	void setTexturesBound(TexturesBound tB) { m_tBound = tB; }

	MaterialProperties getMatPropSet() const { return m_matPropSet; }
	void setMatPropSet(MaterialProperties mPS) { m_matPropSet = mPS; }

	MaterialColors getMatColSet() const { return m_matColSet; }
	void setMatColSet(MaterialColors mCS) { m_matColSet = mCS; }

	bool getHasMoved() const { return m_hasMoved; }
	void setHasMoved(bool hM) { m_hasMoved = hM; }

private:
	glm::mat4 m_modelMatrix	 = glm::mat4(1.0f);
	glm::mat3 m_normalMatrix = glm::mat3(1.0f);
	TexturesBound m_tBound;
	MaterialProperties m_matPropSet;
	MaterialColors m_matColSet;

	// Properties
	bool m_hasMoved = 0;
	bool m_RotationOn = 0;


};