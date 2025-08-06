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


/*
MeshBatch mBatch;
mBatch.indicesStart = indexOffset;

for (unsigned int j = 0; j < meshes.size(); j++)
{
	unsigned int vertexOffset = indexOffset;

	if (meshes[j].textureIds[0] == texturesLoaded[i].id)
	{
		mBatch.texId = texturesLoaded[i].id;

		for (unsigned int k = 0; k < meshes[j].vertices.size(); k++)
		{
			vData.push_back(meshes[j].vertices[k]);
		}
		for (unsigned int k = 0; k < meshes[j].indices.size(); k++)
		{
			iData.push_back(meshes[j].indices[k] + vertexOffset);
			mBatch.indicesCount += meshes[j].indices.size();
		}
		vertexOffset += meshes[i].vertices.size();
	}
	indexOffset += vertexOffset;
}

	}
	*/

    // ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <array>

#include "camera.h"
#include "constants.h"
#include "shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "textureClass.h"
#include "shapes.h"


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

bool mouseInWindow = 1;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f));

int main()
{
    //initialistion boilerplate
    std::cout << "Hello World!\n";
    if (glfwInit() == NULL)
    {
        std::cout << "Window initialisation bungled mate" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 1000, "howdy partner", nullptr, nullptr);
    if (window == NULL)
    {
        std::cout << "Window create fail!" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);

    //this piece of code allows us to use OpenGL functions
    //only call once in the main code
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl; //printing the version 

    /*
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
    */

    //viewport
    unsigned int viewport_width = 1200;
    unsigned int viewport_height = 1000;
    glViewport(0, 0, viewport_width, viewport_height);

    //intialisation boilerplate

    //glfw callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

    //stbi image flip
    stbi_set_flip_vertically_on_load(true);

    // Hide and capture cursor when application has focus
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    <<<<<< < Updated upstream
        Shader lightingShader("res/shaders/lightingShader.vs", "res/shaders/lightingShader.frag");
    Shader lightCubeShader("res/shaders/lightCubeShader.vs", "res/shaders/lightCubeShader.frag");
    ====== =
        // Shader loading
        Shader shader("res/shaders/simple.vs", "res/shaders/simple.fs");
    Shader shaderInstanced("res/shaders/simpleInstanced.vs", "res/shaders/simple.fs");
    Shader cubeMap("res/shaders/cubeMap.vs", "res/shaders/cubeMap.fs");
    >>>>>> > Stashed changes

        std::vector<std::string> milkyway_faces = {
            "skybox/milkyway/right.png",
            "skybox/milkyway/left.png",
            "skybox/milkyway/top.png",
            "skybox/milkyway/bottom.png",
            "skybox/milkyway/front.png",
            "skybox/milkyway/back.png"
    };

    Skybox skybox(milkyway_faces);
    //Skybox skybox(texture_faces);


 /*-----------------------------------------------------------------------------*/
 /*-----------------------------------------------------------------------------*/


    //Vertex Array Object
    VertexArrayObject VAO;

    //Vertex buffer object
    VertexBufferObject VBO;
    VBO.bufferData(cubeNormals);

    //vertex attribute pointers
    VAO.setAttribPointer({ 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0 }); //pass in attribs struct for initialiser list
    VAO.setAttribPointer({ 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) });
    VAO.setAttribPointer({ 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)) });

    //Element buffer object
    //ElementBufferObject EBO;
    //EBO.bufferData(cubeInd);

    VAO.unbind();

    // Light VAO
    VertexArrayObject VAOLight;
    VBO.bind();
    VAOLight.setAttribPointer({ 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0 }); //pass in attribs struct for initialiser list
    VAOLight.unbind();

    // Ground VAO
    VertexArrayObject VAOFloor;
    VertexBufferObject VBOFloor;
    VBOFloor.bufferData(squareNormals);
    VAOFloor.setAttribPointer({ 0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0 });
    VAOFloor.setAttribPointer({ 1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)) });
    VAOFloor.setAttribPointer({ 2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)) });
    VAOFloor.unbind();

    /*==============================================================================================================*/

    Texture texture1({ "textures/container2.png", GL_RGB, GL_TEXTURE_2D, GL_RGBA });
    lightingShader.Use();
    lightingShader.setInt("material.diffuse", 0);

    Texture texture1Spec({ "textures/container2_specular.png", GL_RGB, GL_TEXTURE_2D, GL_RGBA });
    lightingShader.setInt("material.specular", 1);

    Texture texture1Emm({ "textures/matrix.jpg", GL_RGB, GL_TEXTURE_2D, GL_RGB });
    glTextureParameteri(texture1Emm.texId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture1Emm.texId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    lightingShader.setInt("material.emission", 2);

    Texture textureFloor({ "textures/stones.jpg", GL_RGB, GL_TEXTURE_2D, GL_RGB });
    lightingShader.setInt("floorMat.diffuse", 3);
    //glTextureParameteri(textureFloor.texId, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //glTextureParameteri(textureFloor.texId, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    Texture textureFloorSpec({ "textures/stones_specular.jpg", GL_RGB, GL_TEXTURE_2D, GL_RGB });
    lightingShader.setInt("floorMat.specular", 4);
    //glTextureParameteri(textureFloorSpec.texId, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    //glTextureParameteri(textureFloorSpec.texId, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glm::vec3 cubePositions[] = {
    glm::vec3(1.0f,  1.0f,  1.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    /*==============================================================================================================*/

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    double currentTime = glfwGetTime();
    float currentFrame = currentTime;     // get current time 
    const float radius = 2.0f;
    const float rotateSpeed = 10.0f;

    // Camera style
    camera.camFPS = 0;
    camera.camZoom = 60.0f;

    // Point light colors
    glm::vec3 lightColor
        //(0.39, 0.58, 0.93);
        (0.50, 0.45, 0.25);

    // Directional light
    lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
    lightingShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f)); // Suns color
    lightingShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

    // Point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)
    };
    lightingShader.setVec3("pLProperties.pAmbient", lightColor);//glm::vec3(0.05f, 0.05f, 0.05f));
    lightingShader.setVec3("pLProperties.pDiffuse", lightColor); // Point light col
    lightingShader.setVec3("pLProperties.pSpecular", glm::vec3(1.0f, 1.0f, 1.0f));

    lightingShader.setFloat("pLProperties.attConstant", 1.0f);
    lightingShader.setFloat("pLProperties.attLinear", 0.35f);
    lightingShader.setFloat("pLProperties.attQuadratic", 0.44f);

    struct pointLightProperties {
        glm::vec3 pAmbient;
        glm::vec3 pDiffuse;
        glm::vec3 pSpecular;

        float attConstant;
        float attLinear;
        float attQuadratic;
    };

    // Material Shininess
    lightingShader.setFloat("material.shininess", 64.0f);
    lightingShader.setFloat("floorMat.shininess", 2.0f);

    // For flashLight
    lightingShader.setFloat("flashLight.flashAttConstant", 1.0f);
    lightingShader.setFloat("flashLight.flashAttLinear", 0.07f);
    lightingShader.setFloat("flashLight.flashAttQuadratic", 0.017f);


    // Flash light
    lightingShader.setVec3("flashLight.flashAmbient", glm::vec3(0.05f, 0.05f, 0.05f));
    lightingShader.setVec3("flashLight.flashDiffuse", glm::vec3(0.8f, 0.8f, 0.8f));
    lightingShader.setVec3("flashLight.flashSpecular", glm::vec3(1.0f, 1.0f, 1.0f));
    lightingShader.setFloat("flashLight.flashCutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("flashLight.flashOuterCutOff", glm::cos(glm::radians(17.5f)));

    // Point light position
    // point light [0]
    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    // point light [1]
    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    // point light [2]
    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    // point light [3]
    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);

    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);

    glm::vec3 lightPos;
    glm::vec3 lightPosRes[4];


    while (!glfwWindowShouldClose(window))
    {
        // Rendering commands here
        glClearColor(-lightColor.x, -lightColor.y, -lightColor.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        <<<<<< < Updated upstream
            ====== =
            glm::mat4 projection = glm::perspective(glm::radians(camera.camZoom), (float)viewport_width / (float)viewport_height, 0.1f, 300.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(view * model));
        >>>>>> > Stashed changes

            // Make the light source move

            lightPos.x = 1.0f + sin((float)currentTime) * 2.0f;
        lightPos.y = sin((float)currentTime / 2.0f) * 1.0f;
        lightPos.z = 1.0f + sin((float)currentTime) * 2.0f;


        //lightColor.x = sin(currentTime * 2.0f);
        //lightColor.y = sin(currentTime * 0.7f);
        //lightColor.z = sin(currentTime * 1.3f);

        lightCubeShader.Use();
        lightCubeShader.setVec3("lightColor", lightColor);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setMat4("projection", projection);
        // Draw light sources
        for (int i = 0; i < 4; i++)
        {
            <<<<<< < Updated upstream
                lightPosRes[i] = pointLightPositions[i] * lightPos;
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPosRes[i]);
            model = glm::scale(model, glm::vec3(0.20f));
            lightCubeShader.setMat4("model", model);
            VAOLight.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ====== =
                model = glm::mat4(1.0f);
            //model = glm::rotate(model, glm::radians((float)currentTime * rand() * speed), glm::vec3(0.4f, 0.6f, 0.8f));
            model = glm::rotate(model, glm::radians((float)currentTime * speed), glm::vec3(0.0f, 1.0f, 0.0f));
            shaderInstanced.setMat4("model", model);
            glBindVertexArray(rock.meshes[i].VAO);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
            >>>>>> > Stashed changes
        }
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, -2.5f, 0.0f));
        model = glm::scale(model, glm::vec3(0.20f));
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);



        // For cube model apply lighting shader
        lightingShader.Use();
        lightingShader.setInt("u_ObjectType", 0);
        lightingShader.setVec3("viewPos", camera.cameraPos);
        // point light [0]
        lightingShader.setVec3("pointLights[0].position", lightPosRes[0]);
        // point light [1]                                
        lightingShader.setVec3("pointLights[1].position", lightPosRes[1]);
        // point light [2]                                
        lightingShader.setVec3("pointLights[2].position", lightPosRes[2]);
        // point light [3]                                
        lightingShader.setVec3("pointLights[3].position", lightPosRes[3]);
        // static point light[4]
        lightingShader.setVec3("pointLights[4].position", glm::vec3(-7.0f, -2.5f, 0.0f));


        lightingShader.setVec3("pLProperties.pDiffuse", lightColor); // Point light col
        //lightingShader.setVec3("dirLight.diffuse", lightColor);
        //lightingShader.setVec3("dirLight.ambient", lightColor);

                        // Flashlight

        lightingShader.setVec3("flashLight.position", camera.cameraPos);
        lightingShader.setVec3("flashLight.direction", camera.cameraFront);

        if (camera.flashLightOn)
        {
            lightingShader.setInt("u_flashLight", 1);
        }
        else
        {
            lightingShader.setInt("u_flashLight", 0);
        }


        // Materials
        lightingShader.setFloat("time", currentTime);

        // View & Projection transformations
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.camZoom), (float)viewport_width / (float)viewport_height, 0.1f, 100.0f);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        // Draw cubes
        VAO.bind();
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            //model = glm::rotate(model, glm::radians((float)currentTime * ((cubePositions[i].x + 1 + cubePositions[i].z + 1) * 10)), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // Floor
        lightingShader.setInt("u_ObjectType", 1);
        VAOFloor.bind();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(20.0));
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        cubeMap.setMat4("projection", projection);
        cubeMap.setMat4("view", glm::mat4(glm::mat3(camera.getViewMatrix())));    // Eliminating translation factor from mat4
        skybox.draw();

        // Swap the buffers
        glfwSwapBuffers(window);

        // Get Framerate
        currentFrame = glfwGetTime();           // get current time 
        deltaTime = currentFrame - lastFrame;   // difference between the time now, and the time it was when we rendered the last frame  
        lastFrame = currentFrame;

        // Measure speed
        currentTime = glfwGetTime();
        frameCount++;
        // If a second has passed.
        if (currentTime - previousTime >= 1.0)
        {
            // Display the frame count here any way you want.
            std::cout << "                                                                  FPS: " << frameCount << std::endl; // 17 tabs across

            frameCount = 0;
            previousTime = currentTime;
        }

        /*
        std::cout << "Camera position: (" << camera.cameraPos.x << ", "
                                          << camera.cameraPos.y << ", "
                                          << camera.cameraPos.z << ")"
                                          << std::endl;
        */

        // Check and call events
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // When a user presses the escape key, we set the WindowShouldClose property to true, // closing the application

    // Utility
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.processKeyPress(Direction::RIGHT);
    }

    // Mouse context
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (mouseInWindow)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouseInWindow = 0;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            mouseInWindow = 1;
        }

    }

    // Flashlight
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        if (camera.flashLightOn)
        {
            camera.flashLightOn = 0;
        }
        else
        {
            camera.flashLightOn = 1;
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (camera.firstMouse)
    {
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = 0;
    }

    float xoffset = xpos - camera.lastX;           // This frames x-coord, minus the last frames x-coord
    float yoffset = camera.lastY - ypos;           // reversed: y ranges bottom to top
    camera.lastX = xpos;
    camera.lastY = ypos;
    camera.updateEulerValues(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll((float)yoffset);
}




/*
Inversing matrices is a costly operation for shaders, so wherever possible try to avoid
doing inverse operations since they have to be done on each vertex of your scene. For
learning purposes this is fine, but for an efficient application you’ll likely want to calculate
the normal matrix on the CPU and send it to the shaders via a uniform before drawing
(just like the model matrix).
*/

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
