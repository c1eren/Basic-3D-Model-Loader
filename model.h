#pragma once

#include <vector>

#include "mesh.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Model {
public:
	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;

	unsigned int indicesCount = 0;

	Vao VAO;
	Vbo VBO;
	Ebo EBO;

	std::string directory;

public:
	Model(std::string filePath);
	~Model(){}

	void loadModelFromFile(const std::string& filePath);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<unsigned int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	// Fill buffers
	void sendDataToBuffers();

	// Render
	void draw(Shader shader);
	


};