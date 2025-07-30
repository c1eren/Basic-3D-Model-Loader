#include "model.h"
#include <glm/glm.hpp>
#include <glad/glad.h>

#include <iostream>

Model::Model(std::string filePath)
{
	loadModelFromFile(filePath);
	sendDataToBuffers();
}

void Model::loadModelFromFile(const std::string& filePath)
{
	// Assimp importer object, read file with flags
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_Triangulate |
		aiProcess_FlipUVs|
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// Error handling
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// DEBUG
	//std::cout << "Model loaded from file" << std::endl;
	//std::cout << "Meshes: " << scene->mNumMeshes << "\n";
	//std::cout << "Materials: " << scene->mNumMaterials << "\n";
	//std::cout << "Root node name: " << scene->mRootNode->mName.C_Str() << "\n";
	
	// On successful load, process nodes recursively
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Retrieves the aiMesh* pointer from the global mesh list using the index stored in the node.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Store mesh data in temporary vertices buffer
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Position vector
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		// Normal vector
		if (mesh->mNormals)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		// Texture
		// Only using the first texture for now
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 uvVec;
			uvVec.x = mesh->mTextureCoords[0][i].x;
			uvVec.y = mesh->mTextureCoords[0][i].y;
			vertex.uv = uvVec;
		}
		else
		{
			vertex.uv = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}
	
	// Store indices data for rendering
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		if (face.mNumIndices != 3)
		{
			continue; // Skip non triangle faces
		}

		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
			indicesCount++;
		}
	}

	// Return a mesh object and store in <Mesh> vector
	return Mesh(vertices, indices);
}

void Model::sendDataToBuffers()
{
	// TODO clean up logic
	std::vector<Vertex> vData;
	std::vector<unsigned int> iData;
	unsigned int vertexOffset = 0;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		for (unsigned int j = 0; j < meshes[i].vertices.size(); j++)
		{
			vData.push_back(meshes[i].vertices[j]);
		}
		for (unsigned int j = 0; j < meshes[i].indices.size(); j++)
		{
			iData.push_back(meshes[i].indices[j] + vertexOffset);
		}
		vertexOffset += meshes[i].vertices.size(); 
	}

	// Store indices count for rendering
	indicesCount = iData.size();

	// Sending data to GPU
	VAO.bind();
	VBO.addData(vData.data(), vData.size() * sizeof(Vertex));
	EBO.addData(iData.data(), iData.size() * sizeof(unsigned int));
	VAO.setLayout();
	VAO.unbind();
}

void Model::draw(Shader shader)
{
	// Draw model
	shader.use();
	VAO.bind();
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}
