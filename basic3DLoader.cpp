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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

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


    float squareVert[] = {
        // Position          // Tex        // Colors   
       -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
        0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
        0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
       -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,     // Top Left
       -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
        0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f      // Top Right
    };

    Vao VAO;
    Vbo VBO;

    VAO.bind();
    VBO.bind();
    VBO.addData(&squareVert, sizeof(squareVert));
    VAO.setLayout(1, 0, 1, 1, 0);

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
        shader.use();
        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

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

    /*
        void keyCallback (GLFWwindow\* wind, int key, int scancode, int action, int mods) {
        my_input_class.keyCallback(wind, key, scancode, action, mods);
        }
    */
}