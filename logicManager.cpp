#include "logicManager.h"

LogicManager::LogicManager(Camera* camera, ActionManager* actionManager, WindowManager* windowManager)
{
	this->camera = camera;
	this->actionManager = actionManager;
    this->windowManager = windowManager;
}

void LogicManager::setActiveModel(ModelManager* activeModel)
{
	this->activeModel = activeModel;
}

void LogicManager::updateLogic(float deltaTime)
{
    updateApplicationLogic();
	updateCameraLogic(deltaTime);

    if (activeModel)
    {
        getModelState();
        updateModelLogic(deltaTime);
        activeModel->getIsSelected();
        if (!actionManager->isMouseDown(Action::LeftClick) && !isGrabbing)
            activeModel = nullptr;
    }
}

void LogicManager::updateApplicationLogic()
{
    if (actionManager->isActionPressed(Action::SetWindowClose))
        glfwSetWindowShouldClose(windowManager->m_window, true);
}

void LogicManager::getModelState()
{
    checkGrabbing();

    bool leftClick    = actionManager->isMouseDown(Action::LeftClick);
    bool spacePressed = actionManager->isActionDown(Action::MoveUp);
    bool scrolling    = (actionManager->getMouseScrollY() != 0.0f);

    if (activeModel && leftClick)
        activeModel->setIsManipulating(true);
    if (!leftClick)
        activeModel->setIsManipulating(false);

    activeModel->setRotationOn(leftClick && !spacePressed);
    activeModel->setTranslationOn(leftClick && spacePressed);
    activeModel->setScaleOn(leftClick && scrolling);
}

void LogicManager::checkGrabbing()
{
    if (actionManager->isActionPressed(Action::Grab))
    {
        if (!isGrabbing && activeModel)
        {
            isGrabbing = true;
            activeModel->setIsGrabbed(true);
        }
        else if (isGrabbing)
        {
            isGrabbing = false;
            activeModel->setIsGrabbed(false);
        }
    }
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

    //TODO: Have a look at this grabbing code 
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
        if (activeModel->getRotationOn())
        {
            float mouseYaw = actionManager->getMouseMoveYaw();
            rotationY += mouseYaw;
        }

        // Rotation OR translation, but not both simulteneously
        if (activeModel->getTranslationOn())
        {
            // Important to scale unit vectors AFTER they've been used for cross calculations
            glm::vec3 rightVector = camera->getCameraX();
            glm::vec3 upVector    = camera->getCameraY();

            // TODO: Scale velocities by distance
            float factor = glm::clamp((glm::length(position - camera->getCameraPos()) / 100), 0.01f, 2.0f);
            rightVector *= (actionManager->getMouseMoveYaw() * factor);
            upVector    *= (actionManager->getMouseMovePitch() * factor);
            position    += (rightVector + upVector);
        }

        if (activeModel->getScaleOn())
        {
            float yScroll = actionManager->getMouseScrollY();
            scale += yScroll * 0.1f;
            scale = glm::clamp(scale, 0.1f, 10.0f);

            radius = activeModel->getBaseRadius() * scale;
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
	const float rotateSpd = 100.0f;
	float yawDelta        = actionManager->getMouseMoveYaw();
	float pitchDelta      = actionManager->getMouseMovePitch();

    if (!activeModel || !activeModel->getIsManipulating())
    {
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
}