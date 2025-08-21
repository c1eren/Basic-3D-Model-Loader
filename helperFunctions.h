#pragma once

#include <glm/glm.hpp>

void printVec3(std::string text, glm::vec3 vector);
void printMat4(std::string text, glm::mat4 matrix);

void resetState();
void updateState();
void inline toggleBool(bool& property);