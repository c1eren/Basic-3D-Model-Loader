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
#include "sphere.h"
#include "modelManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

void getFramerate(GLFWwindow* window);
unsigned int textureFromFile(const char* str, std::string directory);

int toggle = 0;
bool skyboxDraw = 0;

float cursorX = 0.0f;
float cursorY = 0.0f;
float lastX = 400;
float lastY = 300;
int cameraFrozen = 0;

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Model Loader 3D Basic 2025", nullptr, nullptr);
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
    //Skybox skybox(bigBlue_faces);
    Skybox skybox(milkyway_faces);

    // Shader
    Shader shaders[3];
    Shader shader("shaders/modelShader.vert", "shaders/modelShader.frag");
    Shader skyboxShader("shaders/cubeMap.vert", "shaders/cubeMap.frag");
    Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");
    shaders[0] = shader;
    shaders[1] = skyboxShader;
    shaders[2] = lightShader;

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)viewport_width / (float)viewport_height, 0.1f, 200.0f);
    for (unsigned int i = 0; i < (sizeof(shaders) / sizeof(shaders[0])); i++)
    {
        shaders[i].setMat4("projection", projection);
    }

    const unsigned int PS = 14;
    glm::vec3 pLPosition[PS] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.9f, 0.5f, 0.3f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(0.3f, 0.6f, 0.9f),
        glm::vec3(0.4f, 0.8f, 0.4f),
        glm::vec3(0.3f, 0.3f, 1.0f),
        glm::vec3(1.0f, 0.5f, 0.8f),
        glm::vec3(0.5f, 0.9f, 0.2f),
    };

    // Point lights
    glm::vec3 pLAttenuation(1.0f, 0.14f, 0.07f);
    glm::vec3 pLAmbient(0.2f);
    glm::vec3 pLDiffuse(1.0f, 0.5f, 0.5f);
    glm::vec3 pLSpecular(1.0f);

    for (unsigned int i = 0; i < 5; i++)
    {
        std::string strA = "u_lightSource[";
        std::string strB = "].position";
        std::string strC = "].attenuation";
        std::string strD = "].ambient";
        std::string strE = "].diffuse";
        std::string strF = "].specular";
        std::string num = std::to_string(i);

        std::string pLPos = strA + num + strB;
        std::string pLAtt = strA + num + strC;
        std::string pLAmb = strA + num + strD;
        std::string pLDif = strA + num + strE;
        std::string pLSpe = strA + num + strF;

        shader.setVec3(pLPos.c_str(), pLPosition[i]);
        shader.setVec3(pLAtt.c_str(), pLAttenuation);
        shader.setVec3(pLAmb.c_str(), pLAmbient);
        shader.setVec3(pLDif.c_str(), pLDiffuse);
        shader.setVec3(pLSpe.c_str(), pLSpecular);
    }
    lightShader.setVec3("u_lightColor", pLDiffuse);

    //skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));

    // Generate a sphere
    Sphere sphere(100, 100, 0.1);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_FRAMEBUFFER_SRGB);

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    while (!glfwWindowShouldClose(window))
    {
        //glClearColor(0.4, 0.75, 0.60, 1.0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (camera.hasMoved)
        {
            glm::mat4 view = camera.getViewMatrix();
            shader.setMat4("view", view);
            lightShader.setMat4("view", view);
            skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));    // Eliminating translation factor from mat4
        }
        
        if (modelLoaded.manager->getMouseButtonState())
        {
            modelLoaded.manager->setModelMatrix(glm::rotate(modelLoaded.manager->getModelMatrix(), glm::radians(float(cursorX * 0.5)), glm::vec3(0.0f, 1.0f, 0.0f)));
        }

        modelLoaded.manager->setNormallMatrix(glm::mat3(glm::transpose(glm::inverse(modelLoaded.manager->getModelMatrix()))));
        modelLoaded.draw(shader);

        if (skyboxDraw)
        {
            skybox.draw(skyboxShader);
        }

        for (unsigned int i = 0; i < 5; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pLPosition[i] * pLPosition[i]);
            lightShader.setMat4("model", model);
            sphere.draw(lightShader);
        }

        //modelLoaded.manager->setModelMatrix(glm::mat4(1.0f));

        glfwSwapBuffers(window);

        modelLoaded.manager->setMouseButtonState(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
        cameraFrozen = modelLoaded.manager->getMouseButtonState();

        glfwPollEvents();

        //------------------------------------------------

        getFramerate(window); // Important, also gets delta for camera, TODO separate this logic
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
    if (!cameraFrozen)
    {
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
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        if (skyboxDraw)
            skyboxDraw = 0;
        else
            skyboxDraw = 1;
    }

    /*
        void keyCallback (GLFWwindow\* wind, int key, int scancode, int action, int mods) {
        my_input_class.keyCallback(wind, key, scancode, action, mods);
        }
    */
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    float xoffset = xpos - lastX;           // This frames x-coord, minus the last frames x-coord
    float yoffset = lastY - ypos;           // reversed: y ranges bottom to top
    lastX = xpos;
    lastY = ypos;

    // TODO fix camera 
    if (!cameraFrozen)
    {
        if (camera.firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            camera.firstMouse = 0;
        }

        xoffset = xpos - lastX;
        yoffset = lastY - ypos;
        lastX = xpos;
        lastY = ypos;
        camera.setLastXY(lastX, lastY);
        camera.updateEulerValues(xoffset, yoffset);
    }
    else
    {
        cursorX = xoffset;
        cursorY = yoffset;
    }
}

void getFramerate(GLFWwindow *window)
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
        //std::cout << "                                                                  FPS: " << frameCount << std::endl; // 17 tabs across
        std::string str = "Fps: " + std::to_string(frameCount);
        const char* title = str.c_str();
        glfwSetWindowTitle(window, title);

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