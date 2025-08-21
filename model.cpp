#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>

#include "model.h"
#include "stb_image.h"
#include "globalVariables.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

unsigned int textureFromFile(const char* str, std::string directory);
std::vector<Texture> Model::texturesLoaded;

Model::Model(std::string filePath, bool flipUVs)
{
	this->flipUVs = flipUVs;
	texturesLoaded.reserve(1);
	texturesLoaded.emplace_back(Texture{textureFromFile("defaultTex_diffuse.png", "textures"), "texture_diffuse", "textures/defaultTex_diffuse.png"});

	loadModelFromFile(filePath);
	sendDataToBuffers();

	std::cout << "Model VAO: " << getVAO() << ", meshes: " << getMeshes().size() 
			  << "\n___________________________________________________________________________________________________________________" << std::endl;
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

	// Find folder containing import file
	directory = filePath.substr(0, filePath.find_last_of('/')); // "models/Backpack/backpack.obj" for example, the directory would be "models/Backpack"
	std::cout << "___________________________________________________________________________________________________________________\n" <<
		"|" << directory.substr(0, filePath.find_last_of('/')) << '|' << std::endl;

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

	TextureIds texIds;

	Vertex vertex;
	glm::vec3 vector;

	// Position vector
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;
		
		center += vector;

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

	MaterialProperties tempProp;
	MaterialColors	   tempCols;
	// If mesh has an entry point to scene materials array (-1 if not exists)
	if (mesh->mMaterialIndex >= 0)
	{
		// Get aiMaterial object at that location
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		tempProp	= loadMaterialProperties(material);
		tempCols	= loadMaterialColors(material);

		texIds.ti_diffuse  = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		texIds.ti_specular = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		texIds.ti_normal   = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	}
	else
	{
		texIds.ti_diffuse  = texturesLoaded[0].id;
		texIds.ti_specular = texturesLoaded[0].id;
		texIds.ti_normal   = texturesLoaded[0].id;
	}			
				
	if (!texIds.ti_diffuse)
	{			
		texIds.ti_diffuse  = texturesLoaded[0].id;
		texIds.ti_specular = texturesLoaded[0].id;
		texIds.ti_normal   = texturesLoaded[0].id;
	}

	Mesh rMesh{ vertices, indices, texIds, tempProp, tempCols };
	return rMesh;
}

void Model::sendDataToBuffers()
{
	// Data buffers for GPU
	std::vector<Vertex> vData;
	vData.reserve(verticesCount);

	std::vector<unsigned int> iData;
	iData.reserve(indicesCount);

	unsigned int vertexOffset  = 0;
	unsigned int indicesOffset = 0;

	center /= verticesCount;
	std::cout << "Center: (" << center.x << ", " << center.y << ", " << center.z << std::endl;

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		std::vector<Vertex> meshVertices      = meshes[i].getVertices();
		for (unsigned int j = 0; j < meshVertices.size(); j++)
		{
			glm::vec3 pos = meshVertices[j].position;
			float dist = glm::length(pos - center);
			if (dist > radius)
				radius = dist;
		}

		std::vector<unsigned int> meshIndices = meshes[i].getIndices();

		// Flattening multiple meshes' vertices into a single vertex buffer.
		vData.insert(vData.end(), meshVertices.begin(), meshVertices.end());
		iData.insert(iData.end(), meshIndices.begin(),  meshIndices.end());

		TrackIndices tInd{ indicesOffset * sizeof(unsigned int),
							meshIndices.size(),
							vertexOffset };
		meshes[i].setTrackIndices(tInd);

		indicesOffset += meshIndices.size();
		vertexOffset  += meshVertices.size();
	}
	std::cout << "Radius: " << radius << std::endl;

	// Sending data to GPU
	VAO.bind();
	VBO.addData(vData.data(), vData.size() * sizeof(Vertex));
	EBO.addData(iData.data(), iData.size() * sizeof(unsigned int));
	VAO.setLayout();
	VAO.unbind();
}

unsigned int Model::textureFromFile(const char* str, std::string directory)
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
		return texturesLoaded[0].id;

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
				texturesLoaded.push_back(Texture{ texId, typeName, str.C_Str(), samplerCounter - 1 });
			}
		}
	}

	if (!texId)
		texId = texturesLoaded[0].id;

	return texId;
}

MaterialProperties Model::loadMaterialProperties(const aiMaterial* material)
{
	float shininess;
	float opacity;

	if (AI_SUCCESS != material->Get(AI_MATKEY_SHININESS, shininess))
		std::cout << "Error collecting SHININESS" << std::endl;

	if (AI_SUCCESS != material->Get(AI_MATKEY_OPACITY, opacity))
		std::cout << "Error collecting OPACITY" << std::endl;

	return { shininess, opacity };
}

MaterialColors Model::loadMaterialColors(const aiMaterial* material)
{
	aiColor3D color_ambient;
	aiColor3D color_diffuse;
	aiColor3D color_specular;
	aiColor3D color_emissive;
	aiColor3D color_transparent;

	glm::vec3 ambient(0.0f);
	glm::vec3 diffuse(0.0f);
	glm::vec3 specular(0.0f);
	glm::vec3 emissive(0.0f);
	glm::vec3 transparent(0.0f);

	if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_AMBIENT, color_ambient))
		std::cout << "Error collecting COLOR_AMBIENT" << std::endl;
	else
		ambient = glm::vec3(color_ambient.r, color_ambient.b, color_ambient.g);

	if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_DIFFUSE, color_diffuse))
		std::cout << "Error collecting COLOR_DIFFUSE" << std::endl;
	else
		diffuse = glm::vec3(color_diffuse.r, color_diffuse.b, color_diffuse.g);

	if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_SPECULAR, color_specular))
		std::cout << "Error collecting COLOR_SPECULAR" << std::endl;
	else
		specular = glm::vec3(color_specular.r, color_specular.b, color_specular.g);

	if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_EMISSIVE, color_emissive))
		std::cout << "Error collecting COLOR_EMISSIVE" << std::endl;
	else
		emissive = glm::vec3(color_emissive.r, color_emissive.b, color_emissive.g);

	if (AI_SUCCESS != material->Get(AI_MATKEY_COLOR_TRANSPARENT, color_transparent))
		std::cout << "Error collecting COLOR_TRANSPARENT" << std::endl;
	else
		transparent = glm::vec3(color_transparent.r, color_transparent.b, color_transparent.g);

	return { ambient, diffuse, specular, emissive, transparent };
}