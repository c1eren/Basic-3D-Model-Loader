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
    if (!activeModel || !actionManager)
        return;

    glm::mat4 model(1.0f);
    glm::vec3 position = activeModel->getPosition();
    float rotationY    = activeModel->getRotationY();
    float scale        = activeModel->getScale();
    float radius       = activeModel->getRadius();

    if (activeModel->getIsGrabbed())
    {
        glm::vec3 cameraPos   = camera->getCameraPos();
        glm::vec3 cameraFront = camera->getCameraFront();
        float grabDistance    = activeModel->getRadius() * 1.2f;

        //Factor grab distance by radius of model so it remains out of near plane
        position = cameraPos + cameraFront * grabDistance;
    }

    if (activeModel->getIsManipulating())
    {
        //if (!getTranslationOn())
        //    position = camera.cameraPos + camera.cameraFront;

        if (activeModel->getRotationOn())
        {
            float mouseYaw = actionManager->getMouseMoveYaw();
            rotationY += mouseYaw;
        }

        // Rotation OR translation, but not both simulteneously
        if (activeModel->getTranslationOn() && !activeModel->getIsGrabbed())
        {
            //grabPress = 0;
            // Important to scale unit vectors AFTER they've been used for cross calculations
            glm::vec3 rightVector = camera->getCameraX();
            glm::vec3 upVector    = camera->getCameraY();

            // TODO: Scale velocities by distance
            float factor = glm::clamp((glm::length(position - camera->getCameraPos()) / 100), 0.01f, 2.0f);
            rightVector *= (xVelocity * factor);
            upVector    *= (yVelocity * factor);
            position    += (rightVector + upVector);
        }

        if (activeModel->getScaleOn())
        {
            float yScroll = actionManager->getMouseScrollY();
            if (scale < 1)
                scale += yScroll * scale;
            else
                scale += yScroll;
            radius *= scale;
        }
    }

    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));
    activeModel->setRadius(radius);
    activeModel->setPosition(position);
    activeModel->setRotationY(rotationY);
    activeModel->setScale(scale);
    activeModel->setModelMatrix(model);
    activeModel->setNormalMatrix(glm::mat3(glm::transpose(glm::inverse(model))));
    activeModel->setHasMoved(1);
}

void LogicManager::updateCameraLogic(float deltaTime)
{
	if (!camera || !actionManager)
		return;

	const float moveSpeed = 5.0f * deltaTime;
	const float rotateSpd = 0.1f;
	float yawDelta        = actionManager->getMouseMoveYaw();
	float pitchDelta      = actionManager->getMouseMovePitch();

	if (actionManager->isActionDown(Action::MoveForward))
		camera->moveForward(moveSpeed);
	if (actionManager->isActionDown(Action::MoveBackward))
		camera->moveBackward(moveSpeed);
	if (actionManager->isActionDown(Action::MoveLeft))
		camera->moveLeft(moveSpeed);
	if (actionManager->isActionDown(Action::MoveRight))
		camera->moveRight(moveSpeed);
	if (actionManager->isActionDown(Action::MoveUp))
		camera->moveUp(moveSpeed);
	if (actionManager->isActionDown(Action::MoveDown))
		camera->moveDown(moveSpeed);

	if (yawDelta != 0.0f)
		camera->rotateYaw(yawDelta * rotateSpd * deltaTime);
	if (pitchDelta != 0.0f)
		camera->rotatePitch(pitchDelta * rotateSpd * deltaTime);

	if (camera->getHasRotated())
		camera->updateCameraFrontVectors();
}