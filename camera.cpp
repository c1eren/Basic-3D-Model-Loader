#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include "globalVariables.h"

Camera::Camera()
{
    updateCameraFrontVectors();
}

void Camera::moveForward(const float speed)
{
    cameraPos += cameraFront * speed;
    hasMoved = 1;
}

void Camera::moveBackward(const float speed)
{
    cameraPos -= cameraFront * speed;
    hasMoved = 1;
}

void Camera::moveLeft(const float speed)
{
    cameraPos -= cameraX * speed;
    hasMoved = 1;
}

void Camera::moveRight(const float speed)
{
    cameraPos += cameraX * speed;
    hasMoved = 1;
}

void Camera::moveUp(const float speed)
{
    cameraPos += worldUp * speed;
    hasMoved = 1;
}

void Camera::moveDown(const float speed)
{
    cameraPos -= worldUp * speed;
    hasMoved = 1;
}

void Camera::rotateYaw(const float sensitivity)
{
    yaw += xOffset * sensitivity;
}

void Camera::rotatePitch(const float sensitivity)
{
    pitch += yOffset * sensitivity;

    // Pitch constraints
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}


void Camera::processMouseScroll(float yoffset)
{
    camZoom -= yoffset / camSensitivity;

    if (camZoom < 1.0f)
        camZoom = 1.0f;
    if (camZoom > 90.0f)
        camZoom = 90.0f;
}

void Camera::updateEulerValues(float xoffset, float yoffset)
{
    hasMoved = 1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Pitch constraints
    if (pitch > 89.0f)   
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    updateCameraFrontVectors();
}

void Camera::updateCameraFrontVectors()
{
    glm::vec3 direction;

    direction.x = cos(glm::radians(camYaw)) * cos(glm::radians(camPitch));
    direction.y = sin(glm::radians(camPitch));
    direction.z = sin(glm::radians(camYaw)) * cos(glm::radians(camPitch));

    cameraFront = glm::normalize(direction);
    cameraX = glm::normalize(glm::cross(cameraFront, worldUp));                 // Get perpendicular axis, relative to the camera in the world
    cameraY = glm::normalize(glm::cross(cameraX, cameraFront)); 
}


glm::mat4 Camera::getViewMatrix()
{
    hasMoved = 0;
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraY);
}

//glm::mat4 Camera::lookAT()
//{
//    // Get z,x,y axis 
//    glm::vec3 cameraDirection = glm::normalize(-cameraFront); 
//    glm::vec3 cameraX         = glm::normalize(glm::cross(worldUp, cameraDirection));
//    glm::vec3 cameraY         = glm::cross(cameraDirection, cameraX);
//
//    // Translation matrix
//    glm::mat4 translation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
//                                      0.0f, 1.0f, 0.0f, 0.0f,
//                                      0.0f, 0.0f, 1.0f, 0.0f,
//                                      -cameraPos.x, -cameraPos.y, -cameraPos.z, 1.0f);
//
//    // Create rotation matrix
//    glm::mat4 rotation = glm::mat4(cameraX.x, cameraY.x, cameraDirection.x, 0.0f,
//        cameraX.y, cameraY.y, cameraDirection.y, 0.0f,
//        cameraX.z, cameraY.z, cameraDirection.z, 0.0f,
//        0.0f, 0.0f, 0.0f, 1.0f);
//
//    return rotation * translation;
//}

