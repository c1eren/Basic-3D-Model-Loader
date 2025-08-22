#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include "inputManager.h"

enum class Action {
	MoveForward,
	MoveBackward,
	MoveLeft,
	MoveRight,
	MoveUp,
	MoveDown,
	SetWindowClose,
	LeftClick,
	Grab
};

class ActionManager {
private:
	std::map<Action, int> actionKeyMap;
public:
	ActionManager();
	~ActionManager() {}

	// Key actions
	bool isActionPressed(Action action);
	bool isActionDown(Action action);

	// Mouse actions
	bool isMousePressed(Action action); // Keep an eye on this, might need its own enum class
	bool isMouseDown(Action action);
	float getMouseMoveYaw();
	float getMouseMovePitch();
	float getMouseScrollY();
};



