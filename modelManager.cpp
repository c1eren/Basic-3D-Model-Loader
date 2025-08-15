#include "modelManager.h"
#include "globalVariables.h"

void ModelManager::move(Camera* camera)
{
    if (getIsSelected())
    {
        glm::mat4 model(1.0f);
        glm::vec3 position = getPosition();
        float rotationY = getRotationY();
        float scale = getScale();

        if (getIsGrabbed())
        {
            if (!getTranslationOn())
                position = camera->cameraPos + camera->cameraFront;
        }

        if (getRotationOn())
        {
            rotationY += xVelocity;
        }

        // Rotation OR translation, but not both simulteneously
        if (getTranslationOn())
        {
            grabPress = 0;
            // Important to scale unit vectors AFTER they've been used for cross calculations
            glm::vec3 rightVector = camera->cameraX;
            glm::vec3 upVector = camera->cameraY;

            // Scale velocities by distance
            float factor = glm::max(glm::length(position - camera->cameraPos) / 100.0f, 0.002f);
            rightVector *= (xVelocity * factor);
            upVector *= (yVelocity * factor);
            position += (rightVector + upVector);
        }

        if (getScaleOn())
        {
            // Superset prevents (I think) 0 out or negative scale 
            if (scale < 1)
                scale += yScroll * scale;
            else
                scale += yScroll;
        }

        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale));
        setPosition(position);
        setRotationY(rotationY);
        setScale(scale);
        setModelMatrix(model);
        setNormalMatrix(glm::mat3(glm::transpose(glm::inverse(getModelMatrix()))));

        setHasMoved(1);
        xVelocity = 0.0f;
        yVelocity = 0.0f;
        yScroll = 0.0f;
    }
}