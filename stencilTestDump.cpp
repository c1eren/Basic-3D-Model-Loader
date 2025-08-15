// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"  

#include <iostream>
#include <array>

#include "camera.h"
#include "constants.h"
#include "shader.h"
//#include "model.h"
#include "VAO.h"
#include "VBO.h"
#include "shapes.h"
#include "cube.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void drawTwoCubes(Shader& shader, Cube& cube);
void drawTwoCubesAgain(Shader& shader, Cube& cube);


bool mouseInWindow = 1;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

int main()
{
    //initialistion boilerplate
    std::cout << "Hello World!\n";
    if (glfwInit() == NULL)
    {
        std::cout << "Window initialisation bungled mate" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "howdy partner", nullptr, nullptr);
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

    //intialisation boilerplate

    //glfw callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

    //stbi image flip
    stbi_set_flip_vertically_on_load(true);

    // Hide and capture cursor when application has focus
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // Shader loading
    Shader modelShader("res/shaders/modelShader.vs", "res/shaders/modelShader.fs");
    Shader depthShader("res/shaders/depthShader.vs", "res/shaders/depthShader.fs");
    Shader singleColor("res/shaders/shaderSingleColor.vs", "res/shaders/shaderSingleColor.fs");
    //Shader modelShader("C:/programming/opengl/proj_1/ConsoleApplication1/res/shaders/modelShader.vs", "C:/programming/opengl/proj_1/ConsoleApplication1/res/shaders/modelShader.fs");

    // Model loading
    //Model backpack("models/backpack/backpack.obj");
    //Model tree("models/Tree1/Tree1.obj");

    // Texture paths
    std::vector<std::string> cubeTexPaths = {
        "textures/marble_diffuse.jpg"
    };

    std::vector<std::string> floorTexPaths = {
        "textures/metal_diffuse.png"
    };

    Cube cube(cubeNormals, cubeInd, cubeTexPaths);
    Cube floor(floorVertNorm, floorInd, floorTexPaths);


    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, cubeNormals.size() * sizeof(float), &cubeNormals[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeInd.size() * sizeof(unsigned int), &cubeInd[0], GL_STATIC_DRAW);

    // Vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);


    // FPS and timekeeping
    double currentTime = glfwGetTime();
    float currentFrame = currentTime;     // get current time 
    const float radius = 2.0f;
    const float rotateSpeed = 10.0f;

    // Camera style
    camera.camFPS = 0;
    camera.camZoom = 60.0f;

    // View, projection, model matrices
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat3 normalMatrix = glm::mat3(1.0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // If depth and stencil tests pass, we replace that fragment with our draw call fragment


    //glEnable(GL_CULL_FACE);

    while (!glfwWindowShouldClose(window))
    {
        // Rendering commands here
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // View & Projection transformations
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.camZoom), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);

        singleColor.Use();
        singleColor.setMat4("view", view);
        singleColor.setMat4("projection", projection);

        depthShader.Use();
        depthShader.setMat4("view", view);
        depthShader.setMat4("projection", projection);


        // Don't update stencil buffer while drawing the floor
        glStencilMask(0x00);

        // Floor
        depthShader.setMat4("model", glm::mat4(1.0f));
        floor.Draw(depthShader);

        // Stencil
        glStencilFunc(GL_ALWAYS, 1, 0xFF);         // For the next draw, for every fragment we draw, mark as one (ref)
        glStencilMask(0xFF);                       // 0xFF means we can write to the stencil in the draw call

        // Cubes
        drawTwoCubes(depthShader, cube);

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Basically if what we draw next isn't on top of a box fragment, draw it.
        glStencilMask(0x00);                 // Don't write to stencil buffer with this draw call 
        glDisable(GL_DEPTH_TEST);            // Just to stop our outlines getting drawn over by the floor



        drawTwoCubesAgain(singleColor, cube);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);


        // Swap the buffers
        glfwSwapBuffers(window);


        // Get Framerate
        currentFrame = glfwGetTime();           // get current time 
        deltaTime = currentFrame - lastFrame;   // difference between the time now, and the time it was when we rendered the last frame  
        lastFrame = currentFrame;

        // Measure speed
        currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - previousTime >= 1.0)
        {
            // Display the frame count here any way you want.
            std::cout << "                                                                  FPS: " << frameCount << std::endl; // 17 tabs across

            frameCount = 0;
            previousTime = currentTime;
        }


        // Check and call events
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}
void drawTwoCubesAgain(Shader& shader, Cube& cube)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    model = glm::scale(model, glm::vec3(1.1f));
    shader.setMat4("model", model);
    cube.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.1f));
    shader.setMat4("model", model);
    cube.Draw(shader);
}

void drawTwoCubes(Shader& shader, Cube& cube)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader.setMat4("model", model);
    cube.Draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    cube.Draw(shader);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, // closing the application

    // Utility
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::RIGHT);
    }

    // Mouse context
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (mouseInWindow)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseInWindow = 0;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseInWindow = 1;
        }

    }

    // Flashlight
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (camera.flashLightOn)
        {
            camera.flashLightOn = 0;
        }
        else
        {
            camera.flashLightOn = 1;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera.firstMouse)
    {
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = 0;
    }

    float xoffset = xpos - camera.lastX;           // This frames x-coord, minus the last frames x-coord
    float yoffset = camera.lastY - ypos;           // reversed: y ranges bottom to top
    camera.lastX = xpos;
    camera.lastY = ypos;
    camera.updateEulerValues(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)yoffset);
}




/*
Inversing matrices is a costly operation for shaders, so wherever possible try to avoid
doing inverse operations since they have to be done on each vertex of your scene. For
learning purposes this is fine, but for an efficient application you’ll likely want to calculate
the normal matrix on the CPU and send it to the shaders via a uniform before drawing
(just like the model matrix).
*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
