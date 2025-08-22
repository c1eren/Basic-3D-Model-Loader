#pragma once
#include "modelManager.h"
#include "camera.h"
#include "actionManager.h"

class LogicManager {
public:
	LogicManager(Camera* camera, ActionManager* actionManager);
	~LogicManager() {}

	void updateLogic(float deltaTime);
	void setActiveModel(ModelManager* activeModel);

private:
	ModelManager* activeModel = nullptr;
	Camera* camera = nullptr;
	ActionManager* actionManager = nullptr;

private:
	void updateModelLogic(float *deltaTime);
	void updateCameraLogic(float *deltaTime);
};