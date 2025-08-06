#pragma once

#include <array>
#include <vector>
#include <string>
#include <glad/glad.h>
#include "shader.h"


class Skybox {

public:
    unsigned int m_texID;
    unsigned int VAO;


    std::array<float, 24> m_vertices = {
        // Positions (8 corners)
        -0.5f, -0.5f, -0.5f,  // 0
         0.5f, -0.5f, -0.5f,  // 1
         0.5f,  0.5f, -0.5f,  // 2
        -0.5f,  0.5f, -0.5f,  // 3
        -0.5f, -0.5f,  0.5f,  // 4
         0.5f, -0.5f,  0.5f,  // 5
         0.5f,  0.5f,  0.5f,  // 6
        -0.5f,  0.5f,  0.5f   // 7
    };

    std::array<unsigned int, 36> m_indices = {
        // Back face
        0, 1, 2,
        2, 3, 0,

        // Front face
        4, 5, 6,
        6, 7, 4,

        // Left face
        4, 0, 3,
        3, 7, 4,

        // Right face
        1, 5, 6,
        6, 2, 1,

        // Bottom face
        4, 5, 1,
        1, 0, 4,

        // Top face
        3, 2, 6,
        6, 7, 3
    };

public:
    Skybox(std::vector<std::string> faces);
    void makeBox();
    void loadCubemap(std::vector<std::string> faces);
    void draw(Shader shader);
};

