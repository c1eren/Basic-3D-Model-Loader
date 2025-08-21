#include <iostream>
#include <string>
#include "helperFunctions.h"

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