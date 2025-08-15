// ConsoleApplication2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION
#define NR_POINT_LIGHTS 3

#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "globalVariables.h"
#include "renderer.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "constants.h"
#include "skybox.h"
#include "sphere.h"
#include "modelManager.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void getFramerate(GLFWwindow* window);
unsigned int textureFromFile(const char* str, std::string directory);

// UNDER CONSTRUCTION //


void checkState(RenderTarget* rt);
void checkTheta(RenderTarget* rt);

// UNDER CONSTRUCTION //

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));
Renderer renderer;

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
    std::cout << "Max sampler2D units: " << maxTextures << std::endl; // 32 on this pc
    */

    //viewport
    unsigned int viewport_width = 800;
    unsigned int viewport_height = 600;
    glViewport(0, 0, viewport_width, viewport_height);

    //glfw callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Hide and capture cursor when application has focus
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Shader
    Shader modelShader("shaders/modelShader.vert", "shaders/modelShader.frag");
    Shader skyboxShader("shaders/cubeMap.vert", "shaders/cubeMap.frag");
    Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");

    glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)viewport_width / (float)viewport_height, 0.1f, 200.0f);
    modelShader.setMat4("projection", projection);
    skyboxShader.setMat4("projection", projection);
    lightShader.setMat4("projection", projection);

    float start = glfwGetTime();

    // Model
    Model model3("models/backpack/backpack.obj", 0);
    Model model1("models/planet/planet.obj");
    //Model model1("models/Tree1/Tree1.obj");
    Model model2("models/abandonedHouse/cottage_obj.obj");
    //Model model1("models/53-cottage_fbx/cottage_fbx.fbx");
    float finish = glfwGetTime();

    std::cout << "                                                            Total model load time: " << finish - start << std::endl;
    renderer.createRenderTarget(&model1, &modelShader);
    renderer.createRenderTarget(&model2, &modelShader);
    renderer.createRenderTarget(&model3, &modelShader);

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

    // Directional
    //glm::vec3 direction(-0.2f, -1.0f, -0.3f);
    glm::vec3 direction(0.497094, -0.353474, 0.792435); // Sun in skybox direction
    glm::vec3 ambient(0.15f, 0.15f, 0.15f);
    glm::vec3 diffuse(0.8f, 0.8f, 0.8f);
    glm::vec3 specular(0.5f, 0.5f, 0.5f);

    modelShader.setVec3("dirLight.direction", direction);
    modelShader.setVec3("dirLight.ambient", ambient);
    modelShader.setVec3("dirLight.diffuse", diffuse);
    modelShader.setVec3("dirLight.specular", specular);

    const unsigned int PS = 14;
    glm::vec3 pLPosition[PS] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 2.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.0f),
        glm::vec3(11.0f, 0.0f, 1.0f),
        glm::vec3(10.0f, 1.0f, 1.0f),
        glm::vec3(11.0f, 1.0f, 1.0f),
        glm::vec3(10.9f, 0.5f, 0.3f),
        glm::vec3(10.5f, 0.5f, 0.5f),
        glm::vec3(10.3f, 0.6f, 0.9f),
        glm::vec3(10.4f, 0.8f, 0.4f),
        glm::vec3(10.3f, 0.3f, 1.0f),
        glm::vec3(11.0f, 0.5f, 0.8f),
        glm::vec3(10.5f, 0.9f, 0.2f),
    };

    // Point lights
    glm::vec3 pLAttenuation(1.0f, 0.14f, 0.07f);
    glm::vec3 pLAmbient(0.2f);
    glm::vec3 pLDiffuse(0.0f, 0.5f, 0.5f);
    glm::vec3 pLSpecular(1.0f);

    // TODO make point light container with dynamic creation and static count tracker

    for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
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

        modelShader.setVec3(pLPos.c_str(), pLPosition[i]);
        modelShader.setVec3(pLAtt.c_str(), pLAttenuation);
        modelShader.setVec3(pLAmb.c_str(), pLAmbient);
        modelShader.setVec3(pLDif.c_str(), pLDiffuse);
        modelShader.setVec3(pLSpe.c_str(), pLSpecular);
    }
    lightShader.setVec3("u_lightColor", pLDiffuse);

    //skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));

    // Generate a sphere
    Sphere sphere(100, 100, 0.1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //glEnable(GL_FRAMEBUFFER_SRGB);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // If depth and stencil tests pass, we replace that fragment with our draw call fragment

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    while (!glfwWindowShouldClose(window))
    {
        //glClearColor(0.4, 0.75, 0.60, 1.0);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glStencilMask(0x00); // Don't update stencil drawing other stuff

        for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            pLDiffuse = glm::vec3(sin(currentFrame));
            lightShader.setVec3("u_lightColor", pLDiffuse);
            modelShader.setVec3("u_lightSource[0].position", pLDiffuse);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pLDiffuse);
            lightShader.setMat4("model", model);
            sphere.draw(lightShader);
        }

        if (camera.hasMoved)
        {
            glm::mat4 view = camera.getViewMatrix();
            modelShader.setMat4("view", view);
            lightShader.setMat4("view", view);
            skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));    // Eliminating translation factor from mat4
        }
        
        // Model handling WIP
        for (unsigned int i = 0; i < renderer.r_renderList.size(); i++)
        {
            RenderTarget rTarget = renderer.r_renderList[i];
            ModelManager* manager = rTarget.rt_manager;
            if (manager->getIsSelected())
            {
                glm::mat4 model(1.0f);
                glm::vec3 position = manager->getPosition();
                float rotationY = manager->getRotationY();
                float scale = manager->getScale();
                
                if (manager->getIsGrabbed())
                {
                    if (!manager->getTranslationOn())
                        position = camera.cameraPos + camera.cameraFront;
                }

                if (manager->getRotationOn())
                {
                    rotationY += xVelocity;
                }

                // Rotation OR translation, but not both simulteneously
                if (manager->getTranslationOn())
                {
                    grabPress = 0;
                    // Important to scale unit vectors AFTER they've been used for cross calculations
                    glm::vec3 rightVector = camera.cameraX;
                    glm::vec3 upVector    = camera.cameraY;

                    // Scale velocities by distance
                    float factor = glm::max(glm::length(position - camera.cameraPos) / 100.0f, 0.002f);               
                    rightVector *= (xVelocity * factor);
                    upVector    *= (yVelocity * factor);
                    position += (rightVector + upVector);
                }

                if (manager->getScaleOn())
                {
                    // Superset prevents (I think) 0 out or negative scale 
                    if (scale < 1)
                        scale += yScroll * scale;
                    else
                        scale += yScroll;
                }

                model = glm::translate(model, position);
                model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::scale(model, glm::vec3(scale));
                manager->setPosition(position);
                manager->setRotationY(rotationY);
                manager->setScale(scale);
                manager->setModelMatrix(model);
                manager->setNormalMatrix(glm::mat3(glm::transpose(glm::inverse(manager->getModelMatrix()))));

                manager->setHasMoved(1);
                xVelocity = 0.0f;
                yVelocity = 0.0f;
                yScroll   = 0.0f;
            }
        }

        for (unsigned int i = 0; i < renderer.getRenderList().size(); i++)
        {
            RenderTarget rt = renderer.getRenderList()[i];
            checkState(&rt);
            checkTheta(&rt);
        }        
        if (skyboxDraw)
        {
            glDepthFunc(GL_LEQUAL);
            glDepthMask(GL_FALSE);
            skybox.draw(skyboxShader);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
        }
        renderer.draw();

        

        glfwSwapBuffers(window);

        mouseLeft = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));
        if (mouseLeft && spacePress)
            isHolding = 1;
        else
            isHolding = 0;

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
    if (!mouseLeft)
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

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        spacePress = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        spacePress = 0;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (!grabPress)
            grabPress = 1;
        else
            grabPress = 0;
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
    xOffset = xpos - lastX;
    yOffset = lastY - ypos;

    // TODO fix camera 
    if (!mouseLeft)
    {
        if (camera.firstMouse)
        {
            camera.lastX = xpos;
            camera.lastY = ypos;
            camera.firstMouse = 0;
        }
        camera.updateEulerValues(xOffset, yOffset);
    }
    else
    {
        velocity = xOffset * rSensitivity;
        xVelocity = xOffset * rSensitivity;
        yVelocity = yOffset * rSensitivity;
    }

    lastX = xpos;
    lastY = ypos;
    camera.lastX = xpos;
    camera.lastY = ypos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrolling = 1;
    yScroll = yoffset * scrollSensitivity;
}

void checkState(RenderTarget* rt)
{
    rt->rt_manager->setRotationOn(mouseLeft && !spacePress);
    rt->rt_manager->setTranslationOn(mouseLeft && spacePress);
    rt->rt_manager->setScaleOn(mouseLeft && scrolling);
    rt->rt_manager->setIsGrabbed(grabPress);
}

void checkTheta(RenderTarget* rt)
{
    glm::vec3 viewDir = glm::normalize(camera.cameraPos - rt->rt_manager->getPosition());
    float theta = glm::dot(viewDir, glm::normalize(-camera.cameraFront));

    if (theta > 0.90f)
    {
        if (!isSelected)
        {
            rt->rt_manager->setIsSelected(1);
            isSelected = 1;
        }
    }
    else
    {
        if (!isHolding)
        {
            rt->rt_manager->setIsSelected(0);
            isSelected = 0;
        }
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