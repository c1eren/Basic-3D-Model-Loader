#pragma once
#include "modelManager.h"
#include "camera.h"
#include "actionManager.h"
#include "windowManager.h"

class LogicManager {
public:
	LogicManager(Camera* camera, ActionManager* actionManager, WindowManager* windowManager);
	~LogicManager() {}

	void updateLogic(float deltaTime);
	void setActiveModel(ModelManager* activeModel);

private:
	ModelManager* activeModel	 = nullptr;
	Camera* camera				 = nullptr;
	ActionManager* actionManager = nullptr;
	WindowManager* windowManager = nullptr;

	bool isGrabbing = 0;

private:
	void updateApplicationLogic();
	void checkGrabbing();
	void getModelState();
	void updateModelLogic(float deltaTime);
	void updateCameraLogic(float deltaTime);
};