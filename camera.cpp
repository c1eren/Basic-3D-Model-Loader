#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

#include "globalVariables.h"

Camera::Camera()
{
    updateCameraFrontVectors();
}

bool Camera::getHasMoved()
{
    return hasMoved;
}

bool Camera::getHasRotated()
{
    return hasRotated;
}

glm::vec3 Camera::getCameraPos() const
{
    return cameraPos;
}

glm::vec3 Camera::getCameraFront() const
{
    return cameraZ;
}

glm::vec3 Camera::getCameraX() const
{
    return cameraX;
}

glm::vec3 Camera::getCameraY() const
{
    return cameraY;
}

void Camera::moveForward(const float speed)
{
    cameraPos += cameraZ * speed;
    hasMoved = 1;
}

void Camera::moveBackward(const float speed)
{
    cameraPos -= cameraZ * speed;
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

void Camera::rotateYaw(const float speed)
{
    yaw += speed;
    hasRotated = 1;
}

void Camera::rotatePitch(const float speed)
{
    pitch += speed;

    // Pitch constraints
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    hasRotated = 1;
}

void Camera::updateCameraFrontVectors()
{
    glm::vec3 direction;
    const float yawRad   = glm::radians(yaw);
    const float pitchRad = glm::radians(pitch);

    direction.x = cos(yawRad) * cos(pitchRad);
    direction.y = sin(pitchRad);
    direction.z = sin(yawRad) * cos(pitchRad);

    cameraZ     = glm::normalize(direction);
    cameraX     = glm::normalize(glm::cross(cameraZ, worldUp));
    cameraY     = glm::normalize(glm::cross(cameraX, cameraZ));
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(cameraPos, cameraPos + cameraZ, cameraY);
}


//void Camera::processMouseScroll(float yoffset)
//{
//    camZoom -= yoffset / camSensitivity;
//
//    if (camZoom < 1.0f)
//        camZoom = 1.0f;
//    if (camZoom > 90.0f)
//        camZoom = 90.0f;
//}

//glm::mat4 Camera::lookAT()
//{
//    // Get z,x,y axis 
//    glm::vec3 cameraDirection = glm::normalize(-cameraZ); 
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

