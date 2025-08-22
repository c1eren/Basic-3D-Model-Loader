#include "actionManager.h"

ActionManager::ActionManager()
{
	cameraActionKeyMap = {
		{CameraAction::MoveForward,	 GLFW_KEY_W},
		{CameraAction::MoveBackward, GLFW_KEY_S},
		{CameraAction::MoveLeft,	 GLFW_KEY_A},
		{CameraAction::MoveRight,	 GLFW_KEY_D},
		{CameraAction::MoveUp,		 GLFW_KEY_SPACE},
		{CameraAction::MoveDown,	 GLFW_KEY_LEFT_SHIFT}
	};

}

bool ActionManager::isActionPressed(CameraAction action)
{
	return InputManager::isKeyPressed(cameraActionKeyMap[action]);
}

bool ActionManager::isActionDown(CameraAction action)
{
	return InputManager::isKeyDown(cameraActionKeyMap[action]);
}