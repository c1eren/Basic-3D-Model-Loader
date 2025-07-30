#include <iostream>

#include "model.h"
#include "stb_image.h"  

unsigned int textureFromFile(const char* str, std::string directory);

std::vector<Texture> textures_loaded;

Model::Model(std::string path)
{
	loadModel(path);
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}

}

void Model::loadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}

	// Find folder containing import file
	directory = path.substr(0, path.find_last_of('/')); // "models/Backpack/backpack.obj" for example, the directory would be "models/Backpack"
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all of the current nodes meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// Push this nodes meshes into our mesh vector after processing
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// Once that is done do the same for each of the nodes children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// This is where we build our mesh

	std::vector<Vertex> vertices;
	std::vector<unsigned int>indices;
	std::vector<Texture> textures;

	// Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector;

		// Position vector
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		// Put this vector into our vertex struct
		vertex.Position = vector;

		// Normal vector
		if (mesh->mNormals)
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		// TexCoord vector
		if (mesh->mTextureCoords[0]) // We only want to use the first texture coordinate in the array (up to 8)
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		// Send this vertex struct to vertices array
		vertices.push_back(vertex);

	}

	// Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		// For each face on the model
		aiFace face = mesh->mFaces[i];
		// Get the indices and put them in the array 
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Textures
	// If mesh has an entry point to scene materials array (-1 if not exists)
	if (mesh->mMaterialIndex >= 0)
	{
		// Grab aiMaterial object at that location
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// Create texture struct/s and store them in a vector called diffuseMaps
		std::vector <Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// Repeat with speculars
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		// GetTexture of "type", at position 'i'. aiTextureType_DIFFUSE[i] etc.
		mat->GetTexture(type, i, &str);
		bool skip = 0;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (!std::strcmp(textures_loaded[j].path.data(), str.C_Str()))
			{
				// Mesh will use this texture
				textures.push_back(textures_loaded[j]);
				skip = 1;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.id = textureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();

			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;

}

unsigned int textureFromFile(const char* str, std::string directory)
{
	// Generate texture handle
	unsigned int texId;
	glGenTextures(1, &texId);

	// Load texture using stbi
	std::string fullPath = directory + '/' + str;
	const char* path = fullPath.c_str(); // Can look at stbi_load() just taking a std::string
	//load texture with stb_i
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
			format = GL_RGBA; // Look here if things start breaking

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




//----------------------------

#include "mesh.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();

}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
		{
			number = std::to_string(diffuseNr++);
		}
		else if (name == "texture_specular")
		{
			number = std::to_string(specularNr++);
		}
		// Concatenate type and number together and set a location value to correspond with active texture unit
		shader.setInt((name + number).c_str(), i);//"material." + 
		//shader.setInt(("material." + name + number).c_str(), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// Draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	glBindVertexArray(0);

}

//--------------------------------------

float squareVert[] = {
	// Position          // Tex        // Colors   
   -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
	0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
	0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
   -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,     // Top Left
   -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
	0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f      // Top Right
};

float testVert[] = {
	// Position          // Tex        // Colors   
   -0.4f, -0.4f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
   -0.2f, -0.4f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
   -0.2f, -0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
   -0.4f, -0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,     // Top Left

	0.4f,  0.4f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  1.0f,     // Bottom Left
	0.2f,  0.4f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,     // Bottom Right
	0.2f,  0.2f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,     // Top Right
	0.4f,  0.2f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,  0.0f      // Top Left
};

unsigned int testInd[]{
0, 1, 2,
3, 0, 2,

4, 5, 6,
7, 4, 6
};


Vao VAO;
Vbo VBO;
Ebo EBO;

VAO.bind();
VBO.bind();
VBO.addData(&testVert, sizeof(testVert));
VAO.setLayout(1, 0, 1, 1, 0);
EBO.addData(&testInd, sizeof(testInd));

VAO.unbind();

//-------------------------------------

float cubeCounterClockwiseNormals[] = {
	// back face (0, 0, -1)
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f, // bottom-left
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f, // top-right
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f, // bottom-right
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f, // top-left

	// front face (0, 0, 1)
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f, // bottom-left
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f, 1.0f, // bottom-right
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f, 1.0f, // top-right
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f, // top-left

	// left face (-1, 0, 0)
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f, 0.0f, // top-right
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f, 0.0f, // top-left
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f, 0.0f, // bottom-left
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f, 0.0f, // bottom-right

	// right face (1, 0, 0)
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f, 0.0f, // top-left
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f, 0.0f, // bottom-right
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f,  0.0f, 0.0f, // top-right
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f,  0.0f, 0.0f, // bottom-left

	 // bottom face (0, -1, 0)
	 -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f, 0.0f, // top-right
	  0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f, 0.0f, // top-left
	  0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f, 0.0f, // bottom-left
	 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f, 0.0f, // bottom-right

	 // top face (0, 1, 0)
	 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f, 0.0f, // top-left
	  0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f, 0.0f, // bottom-right
	  0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f, 0.0f, // top-right
	 -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f, 0.0f  // bottom-left
};

unsigned int cubeIndices[] = {
	// back face
	0, 1, 2,
	0, 3, 1,

	// front face
	4, 5, 6,
	4, 6, 7,

	// left face
	8, 9, 10,
	8, 10, 11,

	// right face
	12, 13, 14,
	12, 14, 15,

	// bottom face
	16, 17, 18,
	16, 18, 19,

	// top face
	20, 21, 22,
	20, 22, 23
};

unsigned int vaoTest, vboTest, eboTest;
glGenVertexArrays(1, &vaoTest);
glBindVertexArray(vaoTest);
glGenBuffers(1, &vboTest);
glBindBuffer(GL_ARRAY_BUFFER, vboTest);
glBufferData(GL_ARRAY_BUFFER, sizeof(cubeCounterClockwiseNormals), &cubeCounterClockwiseNormals[0], GL_STATIC_DRAW);
std::cout << sizeof(cubeCounterClockwiseNormals) << std::endl;
glGenBuffers(1, &eboTest);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboTest);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_STATIC_DRAW);
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
glBindVertexArray(0);