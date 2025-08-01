#include "model.h"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "stb_image.h"

#include <iostream>

unsigned int textureFromFile(const char* str, std::string directory);

Model::Model(std::string filePath)
{
	texturesLoaded.push_back(Texture{ textureFromFile("defaultTex_diffuse.png", "textures"), "texture_diffuse", "textures/defaultTex_diffuse.png" });
	loadModelFromFile(filePath);
	sendDataToBuffers();
}

void Model::loadModelFromFile(const std::string& filePath)
{
	std::cout << "loading model" << std::endl;

	// Assimp importer object, read file with flags
	Assimp::Importer importer;
	const aiScene* scene;
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

	std::cout << "model imported" << std::endl;

	// Find folder containing import file
	directory = filePath.substr(0, filePath.find_last_of('/')); // "models/Backpack/backpack.obj" for example, the directory would be "models/Backpack"

	// DEBUG
	std::cout << "Model loaded from file" << std::endl;
	std::cout << "Meshes: " << scene->mNumMeshes << "\n";
	std::cout << "Materials: " << scene->mNumMaterials << "\n";
	std::cout << "Root node name: " << scene->mRootNode->mName.C_Str() << "\n";
	
	// On successful load, process nodes recursively
	processNode(scene->mRootNode, scene);

	// Check textures against cap for batch rendering with textures
	//int maxTextures = 0;
	//glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	//if (texturesLoaded.size() > maxTextures)
	//{
	//	texCapExceeded = 1;
	//}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	std::cout << "processing node" << std::endl;

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
	std::cout << "processing mesh" << std::endl;
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
	std::vector<unsigned int> diffuseMapIds;
	std::vector<unsigned int> specularMapIds;
	std::vector<unsigned int> normalMapIds;
	unsigned int texId = 0;

	// If mesh has an entry point to scene materials array (-1 if not exists)
	if (mesh->mMaterialIndex >= 0)
	{
		// Get aiMaterial object at that location
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		// Create texture struct/s and store them in a vector called diffuseMaps
		diffuseMapIds = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		//textureIds.insert(textureIds.end(), diffuseMapIds.begin(), diffuseMapIds.end());

		specularMapIds = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		//textureIds.insert(textureIds.end(), specularMapIds.begin(), specularMapIds.end());

		normalMapIds = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		//textureIds.insert(textureIds.end(), normalMapIds.begin(), normalMapIds.end());

	}

	if (!diffuseMapIds.empty())
	{
		std::cout << "texId: diffuse" << std::endl;
		texId = diffuseMapIds[0];
		std::cout << texId << "\n";
	}
	else if (!specularMapIds.empty())
	{
		std::cout << "texId: specular" << std::endl;
		texId = specularMapIds[0];
	}
	else if (!normalMapIds.empty())
	{
		std::cout << "texId: normal" << std::endl;
		texId = normalMapIds[0];
	}
	else
	{
		std::cout << "texId: default" << std::endl;
		texId = 1;
	}

	//std::cout << "texturesLoaded[0].id: " << texturesLoaded[0].id << std::endl;

	textureIds.push_back(texId);
	std::cout << "texId: " << texId << std::endl;


	Mesh retMesh{ vertices, indices, textureIds };
	texMap[texId].push_back(retMesh);

	// Return a mesh object and store in <Mesh> vector
	return retMesh;
}

void Model::sendDataToBuffers()
{
	// TODO clean up logic
	std::vector<Vertex> vData;
	std::vector<unsigned int> iData;
	unsigned int vertexOffset = 0;

	for (auto& it : texMap)
	{
		MeshBatch mBatch{ it.first, iData.size(), 0 };
		
		for (auto& mMesh : it.second)
		{
			for (auto& vert : mMesh.vertices)
			{
				vData.push_back(vert);
			}
			for (auto& ind : mMesh.indices)
			{
				iData.push_back(ind + vertexOffset);
			}
			mBatch.indicesCount += mMesh.indices.size();
			vertexOffset += mMesh.vertices.size();
		}
		if (mBatch.indicesCount > 0)
			meshBatches.push_back(mBatch);
	}

	//for (const Texture& tex : texturesLoaded)
	//{
	//	auto it = texMap.find(tex.id);
	//	if (it != texMap.end())
	//	{
	//		MeshBatch mBatch{ tex.id, iData.size(), 0 };
	//		
	//		// Append vertices
	//		for (const Mesh& mesh : it->second)
	//		{
	//			// Add mesh vertices
	//			for (const Vertex& v : mesh.vertices)
	//			{
	//				vData.push_back(v);
	//			}
	//			// Add mesh indices with vertex offset
	//			for (unsigned int idx : mesh.indices)
	//			{
	//				iData.push_back(idx + vertexOffset);
	//			}
	//			// Update batch count and vertex offset
	//			mBatch.indicesCount += mesh.indices.size();
	//			vertexOffset += mesh.vertices.size();
	//		}
	//		if (mBatch.indicesCount > 0)
	//			meshBatches.push_back(mBatch);
	//	}
	//}
	
	//for (unsigned int i = 0; i < texturesLoaded.size(); i++)
	//{
	//	MeshBatch mBatch{texturesLoaded[i].id, iData.size(), 0};
	//	//mBatch.texId = texturesLoaded[i].id;
	//	//mBatch.indicesStart = iData.size();
	//
	//	for (unsigned int j = 0; j < meshes.size(); j++)
	//	{
	//		if (!meshes[j].textureIds.empty() && meshes[j].textureIds[0] == texturesLoaded[i].id) // For now just diffuse textures (or whatever is at [0])
	//		{
	//			// Append vertices
	//			for (unsigned int k = 0; k < meshes[j].vertices.size(); k++)
	//			{
	//				vData.push_back(meshes[j].vertices[k]);
	//			}
	//			for (unsigned int k = 0; k < meshes[j].indices.size(); k++)
	//			{
	//				iData.push_back(meshes[j].indices[k] + vertexOffset);
	//			}
	//			// Update indices count and vertex offset
	//			mBatch.indicesCount += meshes[j].indices.size();
	//			vertexOffset += meshes[j].vertices.size();
	//		}
	//	}
	//
	//	if (mBatch.indicesCount > 0)
	//	{
	//		meshBatches.push_back(mBatch);
	//	}
	//}

	// Store indices count for rendering
	indicesCount = iData.size();

	// Sending data to GPU
	VAO.bind();
	VBO.addData(vData.data(), vData.size() * sizeof(Vertex));
	EBO.addData(iData.data(), iData.size() * sizeof(unsigned int));
	VAO.setLayout();
	VAO.unbind();
	std::cout << "meshBatches.size(): " << meshBatches.size() << std::endl;

	//unsigned int indexOffset = 0;
	//for (unsigned int i = 0; i < meshes.size(); i++)
	//{
	//	for (unsigned int j = 0; j < meshes[i].vertices.size(); j++)
	//	{
	//		vData.push_back(meshes[i].vertices[j]);
	//	}
	//	for (unsigned int j = 0; j < meshes[i].indices.size(); j++)
	//	{
	//		iData.push_back(meshes[i].indices[j] + vertexOffset);
	//	}
	//	vertexOffset += meshes[i].vertices.size(); 
	//}
}

void Model::bindTextures(Shader shader)
{
	for (unsigned int i = 0; i < texturesLoaded.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texturesLoaded[i].id);
		//std::string name = texturesLoaded[i].type;

		std::string number = std::to_string(i);
		std::string name = "u_tex[" + number + "]";

		std::cout << "name: " << name.c_str() << std::endl;

		// Concatenate type and number together and set a location value to correspond with active texture unit
		shader.setInt(name.c_str(), i);
	}
}

void Model::draw(Shader shader)
{
	shader.use();
	VAO.bind();

	// If it's the first draw call and we have less than 32 textures TODO, make dynamic
	//if (firstDraw)
	//{
	//	if (!texCapExceeded)
	//	{
	//		bindTextures(shader);
	//	}
	//	firstDraw = 0;
	//}

	//glBindTexture(GL_TEXTURE_2D, texturesLoaded[1].id);
	
	for (unsigned int i = 0; i < meshBatches.size(); i++)
	{
		//std::cout << "meshBatches.texId: " << meshBatches[i].texId << std::endl;

		glBindTexture(GL_TEXTURE_2D, meshBatches[i].texId);
		glDrawElementsBaseVertex(GL_TRIANGLES, meshBatches[i].indicesCount, GL_UNSIGNED_INT, (void*)(meshBatches[i].indicesStart),0);
	}
	
	// Draw model
	//shader.use();
	//VAO.bind();
	//glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

unsigned int textureFromFile(const char* str, std::string directory)
{
	// Generate texture handle
	unsigned int texId;
	glGenTextures(1, &texId);

	// Load texture using stbi
	std::string fullPath = directory + '/' + str;
	const char* path = fullPath.c_str();
	std::cout << "path: " << path << std::endl;
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
		std::cout << "mat->GetTexture: " << str.C_Str() << std::endl;
		bool skip = 0;

		// Check against loaded textures
		for (unsigned int j = 0; j < texturesLoaded.size(); j++)
		{
			if (!std::strcmp(texturesLoaded[j].path.data(), str.C_Str()))
			{
				// Mesh will use this texture
				textureIds.push_back(texturesLoaded[j].id);
				std::cout << "Texture type:         " << typeName << std::endl;
				std::cout << "texturesLoaded[j].id: " << texturesLoaded[j].id << std::endl;
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
