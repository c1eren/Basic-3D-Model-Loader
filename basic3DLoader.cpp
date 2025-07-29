// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int toggle = 0;

int main()
{
    // Initialistion boilerplate
    if (glfwInit() == NULL)
    {
        std::cout << "Window initialisation bungled mate" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "2D Renderer", nullptr, nullptr);
    if (window == NULL)
    {
        std::cout << "Window create fail!" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    //this piece of code allows us to use OpenGL functions
    //only call once in the main code
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl; //printing the version 

    /*
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    */

    //viewport
    unsigned int viewport_width = 800;
    unsigned int viewport_height = 600;
    glViewport(0, 0, viewport_width, viewport_height);

    //glfw callbacks
    glfwSetKeyCallback(window, key_callback);

    // Hide and capture cursor when application has focus
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader
    Shader shader("shaders/basic.vs", "shaders/basic.fs");

    // TEMPORARY TEXTURE LOADING
    
    // Load image
    int width, height, nrChannels;
    int internalFormat = 0;
    int imageFormat = 0;

    unsigned char* data = stbi_load("textures/container2_diffuse.png", &width, &height, &nrChannels, 0);

    if (nrChannels == 4)
    {
        internalFormat = GL_RGBA;
        imageFormat = GL_RGBA;
    }
    else
    {
        internalFormat = GL_RGB;
        imageFormat = GL_RGB;
    }

    unsigned int texID;
    glGenTextures(1, &texID);

    // Create Texture
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Free image data
    stbi_image_free(data);

    // TEMPORARY TEXTURE LOADING 

    float squareVert[] = {
        // Position          // Tex        // Colors   
       -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
        0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
        0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
       -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,     // Top Left
       -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
        0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f      // Top Right
    };

    float testVert[] = {
        // Position          // Tex        // Colors   
       -0.4f, -0.4f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
       -0.2f, -0.4f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
       -0.2f, -0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
       -0.4f, -0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,     // Top Left

        0.4f,  0.4f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
        0.2f,  0.4f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
        0.2f,  0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
        0.4f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f      // Top Left
    };

    unsigned int testInd[]{
    0, 1, 2,
    3, 0, 2,

    4, 5, 6,
    7, 4, 6
    };


    Vao VAO;
    Vbo VBO;
    Ebo EBO;

    VAO.bind();
    VBO.bind();
    VBO.addData(&testVert, sizeof(testVert));
    VAO.setLayout(1, 0, 1, 1, 0);
    EBO.addData(&testInd, sizeof(testInd));

    VAO.unbind();

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.4, 0.75, 0.60, 1.0);
        //glClearColor(0.235, 0.701, 0.443, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(glm::sin(glfwGetTime()), 0.0f, 0.0f));
        shader.setMat4("model", model);
        shader.setInt("u_tex", 0);
        shader.setInt("toggle", toggle);
        glBindTexture(GL_TEXTURE_2D, texID);
        VAO.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(glm::cos(glfwGetTime()), 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, // closing the application

    // Utility
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        if (toggle == 0)
            toggle = 1;
        else
            toggle = 0;
    }

    /*
        void keyCallback (GLFWwindow\* wind, int key, int scancode, int action, int mods) {
        my_input_class.keyCallback(wind, key, scancode, action, mods);
        }
    */
}