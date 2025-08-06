#include <iostream>

#include <glm/glm.hpp>
#include "stb_image.h"  

#include "skybox.h"

Skybox::Skybox(std::vector<std::string> faces)
{
	loadCubemap(faces);
    makeBox();
}

void Skybox::makeBox() {
    unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
};

void Skybox::draw(Shader shader)
{
    shader.use();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL); // Set because skybox using pos.xyww

	//view = glm::mat4(glm::mat3(camera.getViewMatrix())); // Eliminating translation factor from mat4
    glBindVertexArray(VAO);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Skybox::loadCubemap(std::vector<std::string> faces)
{
    //stbi image flip
    stbi_set_flip_vertically_on_load(false);

    glGenTextures(1, &m_texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texID);

    int width;
    int height;
    int nrChannels;

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

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

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    //stbi image flip
    stbi_set_flip_vertically_on_load(true);

}