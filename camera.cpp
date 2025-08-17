#include "constants.h"
#include "camera.h"

Camera::Camera(glm::vec3 camPosition, glm::vec3 camFront)
    : cameraPos(camPosition), cameraFront(camFront), camYaw(yaw), camPitch(pitch), camZoom(zoom), camSensitivity(sensitivity), camSpeed(0.0f)
{
    updateCameraFrontVectors();
}

void Camera::processKeyPress(Direction dir)
{
    hasMoved = 1;

    //if (camSpeed < maxSpeed)
    //{
    //    camSpeed += accelRate;
    //}
    //else if (camSpeed > maxSpeed)
    //{
    //    camSpeed = maxSpeed;
    //}

    velocity = maxSpeed * deltaTime;

    if (dir == Direction::FORWARD)
    {
        cameraPos += cameraFront * velocity;
    }
    if (dir == Direction::BACKWARD)
    {
        cameraPos -= cameraFront * velocity;
    }
    if (dir == Direction::LEFT)
    {
        cameraPos -= cameraX * velocity;
    }
    if (dir == Direction::RIGHT)
    {
        cameraPos += cameraX * velocity;
    }
    if (dir == Direction::UP)
    {
        cameraPos += worldUp * velocity;
    }
    if (dir == Direction::DOWN)
    {
        cameraPos -= worldUp * velocity;
    }    
}


void Camera::processMouseScroll(float yoffset)
{
    camZoom -= yoffset / camSensitivity;

    if (camZoom < 1.0f)
    {
        camZoom = 1.0f;
    }
    if (camZoom > 90.0f)
    {
        camZoom = 90.0f;
    }
}

void Camera::updateEulerValues(float xoffset, float yoffset)
{
    hasMoved = 1;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camYaw += xoffset;
    camPitch += yoffset;

    // Pitch constraints
    if (camPitch > 89.0f)
    {
        camPitch = 89.0f;
    }
    if (camPitch < -89.0f)
    {
        camPitch = -89.0f;
    }

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

