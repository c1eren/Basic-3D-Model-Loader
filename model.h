#pragma once

#include <vector>

#include "mesh.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

#include <assimp/scene.h>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
	unsigned int samplerUnit = 0;
};

class Model {
public:
	Model(std::string filePath, bool flipUVs = 1);
	~Model(){}

	// Getters
	unsigned int getVAO() const { return VAO.m_id; }
	std::vector<Mesh> &getMeshes()  { return meshes; } // make const later

	// Render
	std::vector<Texture> getTexturesLoaded() { return texturesLoaded; }

	glm::vec3 center = glm::vec3(0.0f);
	float radius = 0.0f;

private:
	std::vector<Mesh> meshes;
	static std::vector<Texture> texturesLoaded;

	Vao VAO;
	Vbo VBO;
	Ebo EBO;

	unsigned int verticesCount = 0;
	unsigned int indicesCount = 0;

	bool flipUVs = 0;
	std::string directory;

private:
	void loadModelFromFile(const std::string& filePath);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	unsigned int textureFromFile(const char* str, std::string directory);

	MaterialProperties loadMaterialProperties(const aiMaterial *material);
	MaterialColors loadMaterialColors(const aiMaterial* material);

	unsigned int loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// Fill buffers
	void sendDataToBuffers();
};