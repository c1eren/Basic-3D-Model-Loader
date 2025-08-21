#include "modelManager.h"
#include "globalVariables.h"
#include "helperFunctions.h"
#include <iostream>

void ModelManager::move(Camera& camera)
{
    glm::mat4 model(1.0f);
    glm::vec3 position = m_position;
    float& rotationY = m_rotationY;
    float& scale = m_scale;

    if (m_isGrabbed)
    {
        position = camera.cameraPos + camera.cameraFront;
    }
    
    if (m_isManipulating)
    {
        //if (!getTranslationOn())
        //    position = camera.cameraPos + camera.cameraFront;

        if (m_rotationOn)
        {
            rotationY += xVelocity;
        }

        // Rotation OR translation, but not both simulteneously
        if (m_translationOn && !m_isGrabbed)
        {
            grabPress = 0;
            // Important to scale unit vectors AFTER they've been used for cross calculations
            glm::vec3 rightVector = camera.cameraX;
            glm::vec3 upVector = camera.cameraY;

            // TODO: Scale velocities by distance
            float factor = glm::clamp((glm::length(position - camera.cameraPos) / 100), 0.01f, 2.0f);
            std::cout << "Factor: " << factor << std::endl;
            rightVector *= (xVelocity * factor);
            upVector *= (yVelocity * factor);
            position += (rightVector + upVector);
        }

        if (m_scaleOn)
        {
            // Superset prevents (I think) 0 out or negative scale 
            if (scale < 1)
                scale += yScroll * scale;
            else
                scale += yScroll;
        }
    }

    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));
    m_newRadius = m_radius * scale; // this is broken I think
    m_position = position;
    m_rotationY = rotationY;
    m_scale = scale;
    m_modelMatrix = model;
    m_normalMatrix = glm::mat3(glm::transpose(glm::inverse(getModelMatrix())));

    m_hasMoved = 1;
    xVelocity = 0.0f;
    yVelocity = 0.0f;
    yScroll = 0.0f;  
}