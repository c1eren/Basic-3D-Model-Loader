#include <glad/glad.h>
#include <glm/glm.hpp>

#include "renderer.h"

#include <iostream>


//void Renderer::createRenderTarget(Model& model, Shader& shader)
//{
//    RenderTarget rt;
//    rt.rt_VAO = model.getVAO();
//    rt.rt_shader = shader;
//    rt.rt_manager = &model.manager;
//    rt.rt_meshes = model.getMeshes();
//
//    addToRenderList(rt);
//}

void Renderer::draw()
{
    drawNoStencil();
    //if (r_stencil)
    //    drawStencil();
    //else
    //    drawNoStencil();
}

//void Renderer::drawStencil()
//{
//    for (auto& it : r_renderList)
//    {
//        RenderTarget& model = &it;
//        ModelManager& manager = model.rt_manager;
//        Shader& shader = model.rt_shader;
//
//        // Bind active textures
//        if (manager.getFirstDraw())
//        {
//            std::cout << "firstDraw" << std::endl;
//            // Could be vec3 but not really a bottleneck right now
//            shader.setInt("u_texture_diffuse", 0);
//            shader.setInt("u_texture_specular", 1);
//            shader.setInt("u_texture_normal", 2);
//            manager.setFirstDraw(0);
//        }
//
//        for (unsigned int j = 0; j < model.rt_meshes.size(); j++)
//        {
//            Mesh& mesh = &model.rt_meshes[j];
//
//            checkTextureBindings(mesh);
//            checkMaterialProperties(mesh, manager, shader);
//            checkMaterialColors(mesh, manager, shader);
//
//            unsigned int indicesCount = mesh.getIndicesCount();
//            unsigned int indicesStart = mesh.getIndicesStart();
//            unsigned int inBaseVertex = mesh.getBaseVertex();
//
//            glBindVertexArray(model.rt_VAO);
//
//            if (manager.getIsSelected())
//            {
//                // Draw object normally and write stencil
//                glStencilFunc(GL_ALWAYS, 1, 0xFF);
//                glStencilMask(0xFF);
//                shader.setMat4("model", manager.getModelMatrix());
//                shader.setInt("u_outline", 0);
//                glDrawElementsBaseVertex(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void&)indicesStart, inBaseVertex);
//
//                // Draw outline where stencil != 1
//                glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//                glStencilMask(0x00);
//                glDisable(GL_DEPTH_TEST);
//
//                shader.setMat4("model", glm::scale(manager.getModelMatrix(), glm::vec3(1.05f)));
//                shader.setInt("u_outline", 1);
//                glDrawElementsBaseVertex(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void&)indicesStart, inBaseVertex);
//
//                // Reset state
//                glEnable(GL_DEPTH_TEST);
//                glStencilFunc(GL_ALWAYS, 1, 0xFF);
//                glStencilMask(0xFF);
//            }
//            else
//            {
//                glStencilMask(0x00);
//                shader.setMat4("model", manager.getModelMatrix());
//                shader.setInt("u_outline", 0);
//                glDrawElementsBaseVertex(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void&)indicesStart, inBaseVertex);
//            }
//        }
//        manager.setHasMoved(0);
//    }
//}

void Renderer::drawNoStencil()
{
    for (auto& it : r_renderList)
    {
        // Bind active textures
        if (it.getFirstDraw())
        {
            std::cout << "firstDraw" << std::endl;
            // Could be vec3 but not really a bottleneck right now
            it.shader.setInt("u_texture_diffuse", 0);
            it.shader.setInt("u_texture_specular", 1);
            it.shader.setInt("u_texture_normal", 2);
            it.shader.setMat4("model", glm::mat4(1.0f));

            it.setFirstDraw(0);
        }

        if (it.getHasMoved())
        {
            it.shader.setMat4("model", it.getModelMatrix());
            //std::cout << "Model has moved: " << it.getRadius() << std::endl;
        }

        if (it.getIsSelected() && !it.getIsManipulating() && !it.getIsGrabbed())
            it.shader.setInt("u_outline", 1);
        else
            it.shader.setInt("u_outline", 0);

        for (unsigned int j = 0; j < it.model.getMeshes().size(); j++)
        {
            Mesh& mesh = it.model.getMeshes()[j];

            checkTextureBindings(mesh);
            checkMaterialProperties(mesh, it, it.shader);
            checkMaterialColors(mesh, it, it.shader);

            unsigned int indicesCount = mesh.getIndicesCount();
            unsigned int indicesStart = mesh.getIndicesStart();
            unsigned int inBaseVertex = mesh.getBaseVertex();

            it.shader.setMat4("model", it.getModelMatrix()); // Fix so we don't have to do this every loop

            glBindVertexArray(it.model.getVAO());
            glDrawElementsBaseVertex(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, (void*)indicesStart, inBaseVertex);
        }

        it.setHasMoved(0);
    }
}

void Renderer::checkTextureBindings(Mesh& mesh)
{
    unsigned int textureIds[3] = { mesh.texIds.ti_diffuse, mesh.texIds.ti_specular, mesh.texIds.ti_normal };

    // Check texture bindings
    if (textureIds[0]    != r_checklist.cl_diffuse
        || textureIds[1] != r_checklist.cl_specular
        || textureIds[2] != r_checklist.cl_normal)
    {
        //std::cout << "newtex" << std::endl;

        glBindTextures(0, 3, textureIds);
        r_checklist.cl_diffuse  = textureIds[0];
        r_checklist.cl_specular = textureIds[1];
        r_checklist.cl_normal   = textureIds[2];
        //std::cout << "texture change" << std::endl;
    }
}

void Renderer::checkMaterialProperties(Mesh& mesh, ModelManager& manager, Shader& shader)
{
    MaterialProperties     mProps = mesh.getMaterialProps();
    MaterialProperties matPropSet = manager.getMatPropSet();

    if (matPropSet.shininess != mProps.shininess || matPropSet.opacity != mProps.opacity)
    {
        shader.setFloat("u_matProps.shininess", mProps.shininess);
        shader.setFloat("u_matProps.opacity", mProps.opacity);
        matPropSet.shininess = mProps.shininess;
        matPropSet.opacity = mProps.opacity;
    }
    manager.setMatPropSet(matPropSet);
}

void Renderer::checkMaterialColors(Mesh& mesh, ModelManager& manager, Shader& shader)
{
    MaterialColors     mCols = mesh.getMaterialCols();
    MaterialColors matColSet = manager.getMatColSet();

    if (matColSet.color_ambient != mCols.color_ambient)
    {
        std::cout << "ambient" << std::endl;
        shader.setVec3("u_matCols.ambient", mCols.color_ambient);
        matColSet.color_ambient = mCols.color_ambient;
    }

    if (matColSet.color_diffuse != mCols.color_diffuse)
    {
        std::cout << "diffuse" << std::endl;
        shader.setVec3("u_matCols.diffuse", mCols.color_diffuse);
        matColSet.color_diffuse = mCols.color_diffuse;
    }

    if (matColSet.color_specular != mCols.color_specular)
    {
        std::cout << "specular" << std::endl;
        shader.setVec3("u_matCols.specular", mCols.color_specular);
        matColSet.color_specular = mCols.color_specular;
    }

    if (matColSet.color_emissive != mCols.color_emissive)
    {
        std::cout << "emissive" << std::endl;
        shader.setVec3("u_matCols.emissive", mCols.color_emissive);
        matColSet.color_emissive = mCols.color_emissive;
    }

    if (matColSet.color_transparent != mCols.color_transparent)
    {
        std::cout << "transparent" << std::endl;
        shader.setVec3("u_matCols.transparent", mCols.color_transparent);
        matColSet.color_transparent = mCols.color_transparent;
    }
    manager.setMatColSet(matColSet);
}