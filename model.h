#pragma once

#include <vector>
#include <unordered_map>

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

struct MeshBatch {
	unsigned int texId;
	unsigned int indicesStart;
	unsigned int indicesCount;
};

class Model {
public:
	std::unordered_map<unsigned int, std::vector<Mesh>> texMap;
	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;
	std::vector<MeshBatch> meshBatches;

	unsigned int indicesCount = 0;

	Vao VAO;
	Vbo VBO;
	Ebo EBO;

	std::string directory;

	bool firstDraw		= 1;
	bool texCapExceeded = 0;
	bool flipUVs		= 1;

public:
	Model(std::string filePath);
	~Model(){}

	void loadModelFromFile(const std::string& filePath);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<unsigned int> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	void bindTextures(Shader shader);

	// Fill buffers
	void sendDataToBuffers();

	// Render
	void draw(Shader shader);
	


};