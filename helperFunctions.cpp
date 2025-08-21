#include <iostream>
#include <string>
#include "helperFunctions.h"
#include "globalVariables.h"
#include "modelManager.h"

void printVec3(std::string text, glm::vec3 vector)
{
    std::cout << text << ": (" << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
}

void printMat4(std::string text, glm::mat4 matrix)
{
    std::string output;
    for (int row = 0; row < 4; ++row)
    {
        output += "[ ";
        for (int col = 0; col < 4; ++col)
        {
            output += std::to_string(matrix[col][row]); // Column-major access
            if (col < 3) output += ", ";
        }
        output += " ]\n";
    }
    std::cout << text << ":\n" << output << std::endl;
}

void resetState()
{
    // Model related
    isHolding   = 0;
    isSelection = 0;

    // Input related
    mouseLeft   = 0;
    spacePress  = 0;
    scrolling   = 0;
    grabPress   = 0;
}

void updateState(ModelManager *&renderList)
{
    //Persistent between loops
    skyboxDraw = 0;
    alreadyGrabbing = 0;

    ////Determined per-loop:
    //m_isManipulating = 0;
    //m_hasMoved = 0;
    //m_rotationOn = 0;
    //m_translationOn = 0;
    //m_scaleOn = 0;
    //m_isSelected = 0;
    //m_isGrabbed = 0;

}

void inline toggleBool(bool &property)
{
    property = !property;
}
