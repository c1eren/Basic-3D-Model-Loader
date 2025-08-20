#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "model.h"
#include "mesh.h"
#include "camera.h"
#include "shader.h"

struct TexturesBound {
	unsigned int diff = 0;
	unsigned int spec = 0;
	unsigned int norm = 0;
};

class ModelManager {
public:
	ModelManager(Model& a_model, Shader& a_shader)
		: model(a_model), shader(a_shader) {
		m_center = model.center;
		m_radius = model.radius;
		m_newRadius = model.radius;
	}
	~ModelManager() {}

	bool getFirstDraw() const { return m_firstDraw; }
	void setFirstDraw(bool fD) { m_firstDraw = fD; }

	bool getIsManipulating() const { return m_isManipulating; }
	void setIsManipulating(bool iM) { m_isManipulating = iM; }

	bool getRotationOn() const { return m_rotationOn; }
	void setRotationOn(bool rO) { m_rotationOn = rO; }

	bool getTranslationOn() const { return m_translationOn; }
	void setTranslationOn(bool tO) { m_translationOn = tO; }

	bool getScaleOn() const { return m_scaleOn; }
	void setScaleOn(bool sO) { m_scaleOn = sO; }

	float getRadius() const { return m_radius; }
	void setRadius(float mR) { m_radius = mR; m_newRadius = mR; }

	float getNewRadius() const { return m_newRadius; }
	void setNewRadius(float nR) { m_newRadius = nR; }

	bool getIsSelected() const { return m_isSelected; }
	void setIsSelected(bool iS) { m_isSelected = iS; }

	bool getIsGrabbed() const { return m_isGrabbed; }
	void setIsGrabbed(bool iG) { m_isGrabbed = iG; }

	bool getRebindRequired() const { return m_rebindRequired; }
	void setRebindRequired(bool rR) { m_rebindRequired = rR; }

	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 mP) { m_position = mP; }

	float getRotationY() const { return m_rotationY; }
	void setRotationY(float rY) { m_rotationY = rY; }

	float getScale() const { return m_scale; }
	void setScale(float mS) { m_scale = mS; }

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

	void move(Camera& camera);

public:
	Model& model;
	static unsigned int m_id;
	unsigned int id = 0;
	Shader& shader;

private:
	glm::mat4 m_modelMatrix = glm::mat4(1.0f);
	glm::mat3 m_normalMatrix = glm::mat3(1.0f);
	static TexturesBound m_tBound; // Static for now for simplicity 
	MaterialProperties m_matPropSet;
	MaterialColors m_matColSet;

	// Properties
	bool m_firstDraw	  = 1;
	bool m_isManipulating = 0;
	bool m_rebindRequired = 0;
	bool m_hasMoved		  = 0;
	bool m_rotationOn	  = 0;
	bool m_translationOn  = 0;
	bool m_scaleOn		  = 0;
	bool m_isSelected	  = 0;
	bool m_isGrabbed	  = 0;

	float m_radius = 0.0f;
	float m_newRadius = 0.0f;

	glm::vec3 m_center = glm::vec3(0.0f);

	// Position | Rotation | Scale
	glm::vec3 m_position = glm::vec3(0.0f);
	float m_rotationY	 = 0.0f;
	float m_scale		 = 1.0f;
};