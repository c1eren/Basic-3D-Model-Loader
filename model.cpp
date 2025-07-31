#include "model.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb_image.h"

#include <iostream>

unsigned int textureFromFile(const char* str, std::string directory);

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

	//const aiScene* scene = importer.ReadFile(filePath,
	//	aiProcess_Triangulate |
	//	aiProcess_FlipUVs
	//	);

	// Error handling
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}

	// Find folder containing import file
	directory = filePath.substr(0, filePath.find_last_of('/')); // "models/Backpack/backpack.obj" for example, the directory would be "models/Backpack"

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
	std::vector<Vertex>		  vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> textureIds;

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

	// Textures
	// If mesh has an entry point to scene materials array (-1 if not exists)
	if (mesh->mMaterialIndex >= 0)
	{
		// Get aiMaterial object at that location
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Create texture struct/s and store them in a vector called diffuseMaps
		std::vector <unsigned int> diffuseMapIds = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textureIds.insert(textureIds.end(), diffuseMapIds.begin(), diffuseMapIds.end());

		std::vector<unsigned int> specularMapIds = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textureIds.insert(textureIds.end(), specularMapIds.begin(), specularMapIds.end());

		std::vector<unsigned int> normalMapIds = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textureIds.insert(textureIds.end(), normalMapIds.begin(), normalMapIds.end());
	}

	// Return a mesh object and store in <Mesh> vector
	return Mesh(vertices, indices, textureIds);
}

void Model::sendDataToBuffers()
{
	// TODO clean up logic
	std::vector<Vertex> vData;
	std::vector<unsigned int> iData;
	unsigned int vertexOffset = 0;

	// for (meshesPerTextureId; i++)
	//		{
	//			vData[i].push_back(meshesPerTextureId[i].meshes[j].vertices[k]);
	//		} // For every mesh that uses this textureID, push it's data and indices start and len onto vData array item 
	// Mini batches, rendered per texture 

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
	VAO.setLayoutTex3();
	VAO.unbind();
}

void Model::draw(Shader shader)
{
	// Draw model
	shader.use();
	VAO.bind();
	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

unsigned int textureFromFile(const char* str, std::string directory)
{
	// Generate texture handle
	unsigned int texId;
	glGenTextures(1, &texId);

	// Load texture using stbi
	std::string fullPath = directory + '/' + str;
	const char* path = fullPath.c_str();
	int width;
	int height;
	int nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 2)
			format = GL_RG;
		else if (nrChannels == 3)
			format = GL_RGB;
		else
			format = GL_RGBA;

		// Generate texture 
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Free image load data
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture at: " << "\"" << path << "\"" << std::endl;
		stbi_image_free(data);
	}

	return texId;
}

std::vector<unsigned int> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<unsigned int> textureIds;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;

		// GetTexture of "type", at position 'i'. aiTextureType_DIFFUSE[i] etc.
		mat->GetTexture(type, i, &str);
		bool skip = 0;

		// Check against loaded textures
		for (unsigned int j = 0; j < texturesLoaded.size(); j++)
		{
			if (!std::strcmp(texturesLoaded[j].path.data(), str.C_Str()))
			{
				// Mesh will use this texture
				textureIds.push_back(texturesLoaded[j].id);
				skip = 1;
				break;
			}
		}

		if (!skip)
		{
			unsigned int texId = textureFromFile(str.C_Str(), directory);

			textureIds.push_back(texId);
			texturesLoaded.push_back(Texture{ texId, typeName, str.C_Str() });
		}
	}

	return textureIds;
}
