#include "model.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb_image.h"

#include <GLFW/glfw3.h>

#include <iostream>

unsigned int textureFromFile(const char* str, std::string directory);

Model::Model(std::string filePath, bool flipUVs)
{
	this->flipUVs = flipUVs;
	texturesLoaded.push_back(Texture{ textureFromFile("defaultTex_diffuse.png", "textures"), "texture_diffuse", "textures/defaultTex_diffuse.png" });

	loadModelFromFile(filePath);
	sendDataToBuffers();
}

void Model::loadModelFromFile(const std::string& filePath)
{
	// Assimp importer object, read file with flags
	Assimp::Importer importer;
	const aiScene* scene;

	// DEBUG for timing
	float start = glfwGetTime();
	if (flipUVs)
	{
		scene = importer.ReadFile(filePath,
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
	}
	else
	{
		scene = importer.ReadFile(filePath,
			aiProcess_CalcTangentSpace |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
	}

	//	float start = glfwGetTime();
	//const aiScene* scene = importer.ReadFile(filePath,
	//	aiProcess_Triangulate |
	//	aiProcess_FlipUVs
	//	);

	float finish = glfwGetTime();
	std::cout << "                                                          Assimp import load time: " << finish - start << std::endl;
	// DEBUG for timing
	float loadStart = glfwGetTime();

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

	float loadFinish = glfwGetTime();
	std::cout << "                                                        Import data handling time: " << loadFinish - loadStart << std::endl;
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
	vertices.reserve(mesh->mNumVertices);

	std::vector<unsigned int> indices;
	indices.reserve(mesh->mNumFaces * 3);

	unsigned int texIds[3];

	Vertex vertex;
	glm::vec3 vector;

	// Position vector
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{

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

		vertices.emplace_back(vertex);
		verticesCount++;
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
			indices.emplace_back(face.mIndices[j]);
			indicesCount++;
		}
	}

	// If mesh has an entry point to scene materials array (-1 if not exists)
	if (mesh->mMaterialIndex >= 0)
	{
		// Get aiMaterial object at that location
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		/*
			aiMaterial* mat = .....
			// The generic way
			if(AI_SUCCESS != mat->Get(<material-key>,<where-to-store>)) {
			// handle epic failure here
			}

			aiString name;
			mat->Get(AI_MATKEY_NAME,name);
			Or for the diffuse color ('color' won't be modified if the property is not set)

			aiColor3D color (0.f,0.f,0.f);
			mat->Get(AI_MATKEY_COLOR_DIFFUSE,color);
		*/

		texIds[0] = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		texIds[1] = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		texIds[2] = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	}
	else
	{
		texIds[0] = 1;
		texIds[1] = 1;
		texIds[2] = 1;
	}

	if (!texIds)
	{
		texIds[0] = 1;
		texIds[1] = 1;
		texIds[2] = 1;
	}

	Mesh rMesh{ vertices, indices, texIds };
	return rMesh;
}

void Model::sendDataToBuffers()
{
	// Data buffers for GPU
	std::vector<Vertex> vData;
	vData.reserve(verticesCount);

	std::vector<unsigned int> iData;
	iData.reserve(indicesCount);

	unsigned int vertexOffset = 0;
	unsigned int indicesOffset = 0;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		// Flattening multiple meshes' vertices into a single vertex buffer.
		vData.insert(vData.end(), meshes[i].vertices.begin(), meshes[i].vertices.end());
		iData.insert(iData.end(), meshes[i].indices.begin(), meshes[i].indices.end());

		meshes[i].indicesStart = indicesOffset * sizeof(unsigned int);
		meshes[i].indicesCount = meshes[i].indices.size();
		meshes[i].baseVertex = vertexOffset;

		indicesOffset += meshes[i].indices.size();
		vertexOffset += meshes[i].vertices.size();
	}

	// Sending data to GPU
	VAO.bind();
	VBO.addData(vData.data(), vData.size() * sizeof(Vertex));
	EBO.addData(iData.data(), iData.size() * sizeof(unsigned int));
	VAO.setLayout();
	VAO.unbind();
}

void Model::draw(Shader shader)
{
	shader.use();
	VAO.bind();

	// Bind active textures
	if (firstDraw)
	{
		shader.setInt("u_texture_diffuse", 0);
		shader.setInt("u_texture_specular", 1);
		shader.setInt("u_texture_normal", 2);
		firstDraw = 0;
	}

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		if (tBound.diff != meshes[i].texIds[0] || tBound.spec != meshes[i].texIds[1] || tBound.norm != meshes[i].texIds[2])
		{
			glBindTextures(0, 3, meshes[i].texIds);

			//tBound.diff = meshes[i].texIds[0];
			//tBound.spec = meshes[i].texIds[1];
			//tBound.norm = meshes[i].texIds[2];
			//std::cout << "new diff id: " << meshes[i].texIds[0]
			//	<< "\n new spec id: " << meshes[i].texIds[1]
			//	<< "\n new norm id: " << meshes[i].texIds[2]
			//	<< std::endl;
		}

		glDrawElementsBaseVertex(GL_TRIANGLES, meshes[i].indicesCount, GL_UNSIGNED_INT, (void*)meshes[i].indicesStart, meshes[i].baseVertex);
	}
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

unsigned int Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	//std::vector<unsigned int> textureIds;
	unsigned int texId = 0;

	if (!mat)
		return 0;

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
				texId = texturesLoaded[j].id;
				skip = 1;
				break;
			}
		}

		if (!skip)
		{
			unsigned int temp = textureFromFile(str.C_Str(), directory);
			if (temp)
			{
				texId = temp;
				texturesLoaded.push_back(Texture{ texId, typeName, str.C_Str() });
			}
		}
	}

	return texId;
}
