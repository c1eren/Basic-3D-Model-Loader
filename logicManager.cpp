#include "logicManager.h"

LogicManager::LogicManager(Camera* camera, ActionManager* actionManager)
{
	this->camera = camera;
	this->actionManager = actionManager;
}

void LogicManager::setActiveModel(ModelManager* activeModel)
{
	this->activeModel = activeModel;
}

void LogicManager::updateLogic(float deltaTime)
{
	updateModelLogic(deltaTime);
	updateCameraLogic(deltaTime);
}

void LogicManager::updateModelLogic(float deltaTime)
{
	
}

void LogicManager::updateCameraLogic(float deltaTime)
{
	if (!camera || !actionManager)
		return;

	const float moveSpeed		  = 5.0f * deltaTime;
	const float rotateSensitivity = 0.1f * deltaTime;

	if (actionManager->isActionDown(CameraAction::MoveForward))
		camera->moveForward(moveSpeed);
	if (actionManager->isActionDown(CameraAction::MoveBackward))
		camera->moveBackward(moveSpeed);
	if (actionManager->isActionDown(CameraAction::MoveLeft))
		camera->moveLeft(moveSpeed);
	if (actionManager->isActionDown(CameraAction::MoveRight))
		camera->moveRight(moveSpeed);
	if (actionManager->isActionDown(CameraAction::MoveUp))
		camera->moveUp(moveSpeed);
	if (actionManager->isActionDown(CameraAction::MoveDown))
		camera->moveDown(moveSpeed);

	if (actionManager->isMouseYaw())
		camera->rotateYaw(actionManager->getMouseMoveYaw() * rotateSensitivity);
	if (actionManager->isMousePitch())
		camera->rotatePitch(actionManager->getMouseMovePitch() * rotateSensitivity);
}