#include "constants.h"
#include "camera.h"

unsigned int Camera::camNumber = 0; // To set camId to new value every time

Camera::Camera(glm::vec3 camPosition, glm::vec3 camFront)
    : cameraPos(camPosition), cameraFront(camFront), camYaw(yaw), camPitch(pitch), camZoom(zoom), camSensitivity(sensitivity), camSpeed(speed)
{
	this->camId = camNumber;
	camNumber++;

    updateCameraFrontVectors();
}

void Camera::processKeyPress(Direction dir)
{
    hasMoved = 1;
    float velocity = camSpeed * deltaTime;

    if (dir == Direction::FORWARD)
    {
        if (camFPS)
        {
            cameraPos += glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)) * velocity; // This works for FPS mode. We set normalized direction values, but only add strength to chosen axis'.
        }
        else
            cameraPos += cameraFront * velocity;
    }
    if (dir == Direction::BACKWARD)
    {
        if (camFPS)
        {
            cameraPos -= glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z)) * velocity;
        }
        else
        {
            cameraPos -= cameraFront * velocity;
        }
    }
    if (dir == Direction::LEFT)
    {
        cameraPos -= cameraX * velocity;
        if (cameraRoll)
        {
            camRoll--;
            if (camRoll < -5.0f)
            {
                camRoll = -5.0f;
            }
        }
    }
    if (dir == Direction::RIGHT)
    {
        cameraPos += cameraX * velocity;
        if (cameraRoll)
        {
            camRoll++;
            if (camRoll > 5.0f)
            {
                camRoll = 5.0f;
            }
        }
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
    /*
    if (cameraRoll && camRoll != 0.0f)
    {
        glm::vec3 finalRight = cameraX;
        glm::vec3 finalUp    = cameraY;

        //roll rotation on camerafront axis
        glm::mat4 rollRotation  = glm::mat4(1.0f);
        rollRotation            = glm::rotate(rollRotation, glm::radians(camRoll), cameraFront);
        finalRight              = glm::mat3(rollRotation) * cameraX;
        finalUp                 = glm::mat3(rollRotation) * cameraY;

        return glm::lookAt(cameraPos, cameraPos + cameraFront, finalUp);
    }
    */
    //return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraY);

    return lookAT();

}

glm::mat4 Camera::lookAT()
{
    // Get z,x,y axis 
    glm::vec3 cameraDirection = glm::normalize(-cameraFront); // Gives Vector direction that points back at the camera, Cameras forward vector is: -cameraDirection; aligns on cameras local z-axis
    glm::vec3 cameraX         = glm::normalize(glm::cross(worldUp, cameraDirection));
    glm::vec3 cameraY         = glm::cross(cameraDirection, cameraX);

    // Create translation matrix
    /*
    glm::mat4 translation = glm::mat4(1.0f); // Identity matrix
    translation[3][0] = -cameraPos.x;
    translation[3][1] = -cameraPos.y;
    translation[3][2] = -cameraPos.z;
    */

    glm::mat4 translation = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                      0.0f, 1.0f, 0.0f, 0.0f,
                                      0.0f, 0.0f, 1.0f, 0.0f,
                                      -cameraPos.x, -cameraPos.y, -cameraPos.z, 1.0f);

    // Create rotation matrix
    glm::mat4 rotation = glm::mat4(cameraX.x, cameraY.x, cameraDirection.x, 0.0f,
        cameraX.y, cameraY.y, cameraDirection.y, 0.0f,
        cameraX.z, cameraY.z, cameraDirection.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    return rotation * translation;
}

