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
#include "windowManager.h"
#include "inputManager.h"
#include "logicManager.h"
#include "actionManager.h"
#include "renderer.h"
#include "shader.h"
#include "model.h"
#include "camera.h"
#include "skybox.h"
#include "sphere.h"
#include "modelManager.h"
#include "helperFunctions.h"

void getFramerate(GLFWwindow* window);

//RenderTarget createRenderTarget(Model* model, Shader* shader);
bool intersectRaySphere(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius, float& tHit);

// Literally the camera
Camera camera;

// Handles window code
WindowManager windowManager;

// Handles inputs and sends to middle layer
InputManager inputManager(windowManager.m_window);

// Turns input data into actions for the logic manager
ActionManager actionManager;

// Handles action requests from ActionManager and updates object data
LogicManager logicManager(&camera, &actionManager, &windowManager);

// Handles modelManagers and takes transformation data from LogicManager
Renderer renderer;

int main()
{
    GLFWwindow* window = windowManager.m_window;

    // Shader
    Shader modelShader("shaders/modelShader.vert", "shaders/modelShader.frag");
    Shader skyboxShader("shaders/cubeMap.vert", "shaders/cubeMap.frag");
    Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");
    Shader crosshairShader("shaders/crosshair.vert", "shaders/crosshair.frag");

    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)viewport_width / (float)viewport_height, 0.1f, 200.0f);
    modelShader.setMat4("projection", projection);
    skyboxShader.setMat4("projection", projection);
    lightShader.setMat4("projection", projection);

    double start = glfwGetTime();

    // Model
    Model model1("models/planet/planet.obj");
    Model model2("models/abandonedHouse/cottage_obj.obj");
    Model model3("models/backpack/backpack.obj", 0);

    //Model model1("models/Tree1/Tree1.obj");
    //Model model1("models/53-cottage_fbx/cottage_fbx.fbx");
    double finish = glfwGetTime();

    std::cout << "                                                            Total model load time: " << finish - start << std::endl;
    //renderer.createRenderTarget(model1, modelShader);
    //renderer.createRenderTarget(&model2, &modelShader);
    //renderer.createRenderTarget(model3, modelShader);

    ModelManager planet(model1, modelShader);
    ModelManager house(model2, modelShader);
    ModelManager backpack(model3, modelShader);

    house.setModelMatrix(glm::scale(glm::translate(house.getModelMatrix(), glm::vec3(10.0f)), glm::vec3(0.10f)));
    //printVec3("House position", house.getPosition());
    //printVec3("Planet position", planet.getPosition());

    renderer.r_renderList.push_back(planet);
    renderer.r_renderList.push_back(backpack);
    renderer.r_renderList.push_back(house);

    printMat4("Planet matrix", planet.getModelMatrix());


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
    Sphere sphere(100, 100, 0.1f);
    Sphere smallSphere(10, 10, 0.01f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE); // Keep an eye on this 

    //glEnable(GL_FRAMEBUFFER_SRGB);

    //glEnable(GL_STENCIL_TEST);
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // If depth and stencil tests pass, we replace that fragment with our draw call fragment

/*#####################################################################################################################################################*/
    // RENDER LOOP
/*#####################################################################################################################################################*/

    while (!glfwWindowShouldClose(window))
    {
        //std::cout << "Start loop" << std::endl;

        //glClearColor(0.4, 0.75, 0.60, 1.0);
        glClearColor(0.37f, 0.37f, 0.38f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//| GL_STENCIL_BUFFER_BIT);
        //glStencilMask(0x00); // Don't update stencil drawing other stuff

        inputManager.update();
        glfwPollEvents();

            // Model handling WIP
            glm::vec3 rayOrigin = camera.getCameraPos();
            glm::vec3 rayDir = camera.getCameraFront();
            float lastHit = 100.0f;
            ModelManager* nearManager = nullptr;

            for (auto& manager : renderer.r_renderList)
            {
                manager.setIsSelected(0);
                float tHit = 0.0f;

                if (intersectRaySphere(rayOrigin, rayDir, manager.getPosition(), manager.getNewRadius(), tHit))
                {
                    if (tHit < lastHit)
                    {
                        nearManager = &manager; // Keep an eye on this
                        lastHit = tHit;
                        //std::cout << "tHit: " << tHit << std::endl;
                    }
                }
            }
            if (nearManager)
            {
                logicManager.setActiveModel(nearManager);
                isSelection = 1;
            }

            logicManager.updateLogic(deltaTime);


        smallSphere.draw(crosshairShader);
        //std::cout << "Crosshair draw" << std::endl;

        for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            pLPosition[i] = glm::vec3(sin(currentFrame));
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pLPosition[i]);
            lightShader.setMat4("model", model);
            sphere.draw(lightShader);
            //std::cout << "Sphere draw" << std::endl;
        }
        // Save all shader variables and send them at the end in batches 

        if (camera.getHasMoved())
        {
            glm::mat4 view = camera.getViewMatrix();
            modelShader.setMat4("view", view);
            lightShader.setMat4("view", view);
            if (skyboxDraw)
                skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));    // Eliminating translation factor from mat4
            //std::cout << "Camera move" << std::endl;
        }

        if (skyboxDraw)
        {
            //glDepthFunc(GL_LEQUAL);
            //glDepthMask(GL_FALSE);
            skybox.draw(skyboxShader);
            //std::cout << "Skybox draw" << std::endl;
            //glDepthMask(GL_TRUE);
            //glDepthFunc(GL_LESS);
        }

        for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            modelShader.setVec3("u_lightSource[0].position", pLPosition[i]);
        }
        renderer.draw();
        //std::cout << "Models draw" << std::endl;

        glfwSwapBuffers(window);

        mouseLeft = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT));


        //------------------------------------------------

        getFramerate(window); // Important, also gets delta for camera, TODO separate this logic

        //std::cout << "End loop" << std::endl;
        //std::cout << "Shader binds per loop: " << Shader::s_shaderBindsPerLoop << std::endl;
        Shader::s_shaderBindsPerLoop = 0;

        if (mouseLeft)
            isHolding = 1;
        else
            isHolding = 0;

        isSelection = 0;

    }
    /*#####################################################################################################################################################*/
        // RENDER LOOP
    /*#####################################################################################################################################################*/

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

//void checkState(ModelManager& manager)
//{
//    manager.setIsManipulating(mouseLeft && manager.getIsSelected());
//    manager.setRotationOn(mouseLeft && !spacePress);
//    manager.setTranslationOn(mouseLeft && spacePress);
//    manager.setScaleOn(mouseLeft && scrolling);
//    
//    if (!alreadyGrabbing)
//        manager.setIsGrabbed(manager.getIsSelected() && grabPress);
//   // if (manager.getIsGrabbed())
//   //     manager.setIsGrabbed(grabPress);
//   // else
//   //      && !alreadyGrabbing);
//}


void getFramerate(GLFWwindow *window)
{
    // Get Framerate
    currentFrame = static_cast<float>(glfwGetTime());           // get current time 
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

bool intersectRaySphere(const glm::vec3 &rayOrigin, const glm::vec3& rayDir, const glm::vec3& sphereCenter, float sphereRadius, float& tHit)
{
    glm::vec3 oc = rayOrigin - sphereCenter;
    // How much are oc(origin - sphere center direction vector) and the rays direction in alignment
    float b = glm::dot(oc, rayDir);
    float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
    // If h < 0 no intersections, if h = 0 ray grazes sphere, if h > 0 ray enters one point and exits another on sphere
    float h = b * b - c;

    if (h < 0.0f)
        return false;
    // Otherwise compute the two intersection solutions
    h = sqrt(h);
    // Closer intersection, t is the distance along the ray
    float t = -b - h;
    // If closer intersection is behind rays origin, use the farther one (ie. we are inside an objects bounding sphere)
    if (t < 0.0f)
        t = -b + h;
    // If both are behind camera, return false
    if (t < 0.0f)
        return false;
    // If valid intersection found, return the distance along the ray
    tHit = t;
    return true;    
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