#include "actionManager.h"

ActionManager::ActionManager()
{
	actionKeyMap = {
		{Action::MoveForward,	 GLFW_KEY_W},
		{Action::MoveBackward,	 GLFW_KEY_S},
		{Action::MoveLeft,		 GLFW_KEY_A},
		{Action::MoveRight,		 GLFW_KEY_D},
		{Action::MoveUp,		 GLFW_KEY_SPACE},
		{Action::MoveDown,		 GLFW_KEY_LEFT_SHIFT},
		{Action::SetWindowClose, GLFW_KEY_ESCAPE},
		{Action::LeftClick,		 GLFW_MOUSE_BUTTON_LEFT},
		{Action::Grab,			 GLFW_KEY_E}
	};
}

bool ActionManager::isActionPressed(Action action)
{
	return InputManager::isKeyPressed(actionKeyMap[action]);
}

bool ActionManager::isActionDown(Action action)
{
	return InputManager::isKeyDown(actionKeyMap[action]);
}

bool ActionManager::isMousePressed(Action action)
{
	return InputManager::isMousePressed(actionKeyMap[action]);

}
bool ActionManager::isMouseDown(Action action)
{
	return InputManager::isMouseDown(actionKeyMap[action]);
}

// Send offset values through action manager 
float ActionManager::getMouseMoveYaw()
{
	return InputManager::getMouseDeltaX();
}

float ActionManager::getMouseMovePitch()
{
	return InputManager::getMouseDeltaY();
}

float ActionManager::getMouseScrollY()
{
	return InputManager::getScrollY();
}

