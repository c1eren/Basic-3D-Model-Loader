// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <vector>

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
#include "model.h"
#include "camera.h"
#include "constants.h"
#include "skybox.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void getFramerate();
unsigned int textureFromFile(const char* str, std::string directory);

int toggle = 0;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

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
    
    int maxTextures = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
    std::cout << "Max sampler2D units: " << maxTextures << std::endl;
    */

    //viewport
    unsigned int viewport_width = 800;
    unsigned int viewport_height = 600;
    glViewport(0, 0, viewport_width, viewport_height);

    //glfw callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Hide and capture cursor when application has focus
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float start = glfwGetTime();

    // Model
    Model modelLoaded("models/backpack/backpack.obj", 0);
    //Model modelLoaded("models/planet/planet.obj");
    //Model modelLoaded("models/Tree1/Tree1.obj");
    //Model modelLoaded("models/abandonedHouse/cottage_obj.obj");
    //Model modelLoaded("models/53-cottage_fbx/cottage_fbx.fbx");

    float finish = glfwGetTime();

    std::cout << "                                                            Total model load time: " << finish - start << std::endl;

    // Cubemap faces
    std::vector<std::string> bigBlue_faces = {
        "skybox/skybox/right.jpg",
        "skybox/skybox/left.jpg",
        "skybox/skybox/top.jpg",
        "skybox/skybox/bottom.jpg",
        "skybox/skybox/front.jpg",
        "skybox/skybox/back.jpg"
    };

    std::vector<std::string> milkyway_faces = {
        "skybox/milkyway/right.png",
        "skybox/milkyway/left.png",
        "skybox/milkyway/top.png",
        "skybox/milkyway/bottom.png",
        "skybox/milkyway/front.png",
        "skybox/milkyway/back.png"
    };

    // Skybox
    Skybox skybox(bigBlue_faces);
    //Skybox skybox(milkyway_faces);

    // Shader
    Shader shaders[2];
    Shader shader("shaders/modelShader.vs", "shaders/modelShader.fs");
    Shader skyboxShader("shaders/cubeMap.vs", "shaders/cubeMap.fs");
    shaders[0] = shader;
    shaders[1] = skyboxShader;

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)viewport_width / (float)viewport_height, 0.1f, 200.0f);
    for (unsigned int i = 0; i < (sizeof(shaders) / sizeof(shaders[0])); i++)
    {
        shaders[i].setMat4("projection", projection);
    }

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.4, 0.75, 0.60, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (camera.hasMoved)
        {
            glm::mat4 view = camera.getViewMatrix();
            shader.setMat4("view", view);
            skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));    // Eliminating translation factor from mat4
        }
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        shader.setMat3("normalMatrix", normalMatrix);

        modelLoaded.draw(shader);

        skybox.draw(skyboxShader);

        glfwSwapBuffers(window);
        glfwPollEvents();

        //------------------------------------------------

        getFramerate(); // Important, also gets delta for camera, TODO separate this logic
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::UP);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::DOWN);
    }

    /*
        void keyCallback (GLFWwindow\* wind, int key, int scancode, int action, int mods) {
        my_input_class.keyCallback(wind, key, scancode, action, mods);
        }
    */
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

void getFramerate()
{
    // Get Framerate
    currentFrame = glfwGetTime();           // get current time 
    deltaTime = currentFrame - lastFrame;   // difference between the time now, and the time it was when we rendered the last frame  
    lastFrame = currentFrame;

    // Measure speed
    currentFrame = currentFrame; //glfwGetTime();
    frameCount++;
    // If a second has passed.
    if (currentFrame - previousTime >= 1.0)
    {
        // Display the frame count here any way you want.
        std::cout << "                                                                  FPS: " << frameCount << std::endl; // 17 tabs across

        frameCount = 0;
        previousTime = currentFrame;
    }
}

/*
*______________________________________________________________________________________________________________________________________________________________________________________
|Problem                               | When it matters                            | Fix
|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
|Different materials/textures per mesh | If the model uses multiple materials	    | Track mesh index ranges (Mesh.indicesStart, Mesh.indexCount) and draw subranges in separate calls
|                                      |                                            |
|Animations or dynamic meshes	       | If the model uses bones or morphs	        | Use a more advanced system (e.g., SkinnedMesh class)
|                                      |                                            |
|Selective rendering	               | If you want to hide/show individual meshes | Store per-mesh indexCount and offset for glDrawElementsBaseVertex or draw subsets manually
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/