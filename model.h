#pragma once

#include <vector>
#include <unordered_map>

#include "mesh.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

struct TexturesBound {
	unsigned int diff = 0;
	unsigned int spec = 0;
	unsigned int norm = 0;
};


class Model {
public:
	Model(std::string filePath, bool flipUVs = 1);
	~Model(){}

	// Render
	void draw(Shader shader);

private:
	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;
	TexturesBound tBound;

	Vao VAO;
	Vbo VBO;
	Ebo EBO;

	unsigned int verticesCount = 0;
	unsigned int indicesCount = 0;
	bool flipUVs = 0;
	bool firstDraw = 1;

	std::string directory;

private:
	void loadModelFromFile(const std::string& filePath);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	unsigned int loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// Fill buffers
	void sendDataToBuffers();
};