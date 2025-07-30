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

class Model {
public:
	std::vector<Mesh> meshes;
	unsigned int indicesCount = 0;

	Vao VAO;
	Vbo VBO;
	Ebo EBO;

public:
	Model(std::string filePath);
	~Model(){}

	void loadModelFromFile(const std::string& filePath);

	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// Fill buffers
	void sendDataToBuffers();

	// Render
	void draw(Shader shader);
	


};