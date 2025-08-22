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
	updateModelLogic(&deltaTime);
	updateCameraLogic(&deltaTime);
}

void LogicManager::updateModelLogic(float *deltaTime)
{
	
}

void LogicManager::updateCameraLogic(float *deltaTime)
{

}