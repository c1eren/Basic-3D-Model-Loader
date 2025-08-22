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
	RotateYaw,
	RotatePitch
};

class ActionManager {
private:
	std::map<CameraAction, int> cameraActionKeyMap;
public:
	ActionManager();
	~ActionManager() {}
	// Key actions
	bool isActionPressed(CameraAction action);
	bool isActionDown(CameraAction action);
	// Mouse actions
	bool isMouseYaw();
	bool isMousePitch();
	float getMouseMoveYaw();
	float getMouseMovePitch();
};



