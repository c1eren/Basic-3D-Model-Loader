#pragma once

#include "modelManager.h"
#include "shader.h"
#include "mesh.h"
#include "model.h"

struct Checklist {
    unsigned int cl_diffuse = 0;
    unsigned int cl_specular = 0;
    unsigned int cl_normal = 0;
};

//struct RenderTarget {
//    unsigned int rt_VAO = 0;
//    Shader& rt_shader;
//    ModelManager& rt_manager;
//    std::vector<Mesh> rt_meshes;
//};

class Renderer {
public:
    //void createRenderTarget(Model& model, Shader& shader);
    //void addToRenderList(RenderTarget r_target) { r_renderList.reserve(1); r_renderList.emplace_back(r_target); }
    //std::vector<RenderTarget> getRenderList() { return r_renderList; } // Needs fixing

    void setStencil(bool sT) { r_stencil = sT; }
    bool getStencil() { return r_stencil; }

    void draw();
    Renderer() {}
    ~Renderer() {}

public:
    std::vector<ModelManager> r_renderList;

private:
    Checklist r_checklist;
    unsigned int r_boundVAO = 0;
    bool r_stencil = 0;
    

private:
    void checkTextureBindings(Mesh& mesh);
    void checkMaterialProperties(Mesh& mesh, ModelManager& manager, Shader& shader);
    void checkMaterialColors(Mesh& mesh, ModelManager& manager, Shader& shader);

    //void drawStencil();
    void drawNoStencil();
};