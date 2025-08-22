#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include "inputManager.h"

enum class CameraAction {
	MoveForward,
	MoveBackward,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	RotateLeft,
	RotateRight,
	RotateUp,
	RotateDown
};

class ActionManager {
private:
	std::map<CameraAction, int> cameraActionKeyMap;
public:
	ActionManager();
	~ActionManager() {}
	bool isActionPressed(CameraAction action);
	bool isActionDown(CameraAction action);
};



