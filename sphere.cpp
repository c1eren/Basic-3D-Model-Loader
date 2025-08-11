#include <glad/glad.h>
#include "sphere.h"
#include <glm/glm.hpp>


Sphere::Sphere(unsigned int stacks, unsigned int slices, float factor)
{
	for (unsigned int i = 0; i <= stacks; i++)
	{
		float V = (float)i / (float)stacks;
        float phi = V * PI;

        // loop through the slices.
        for (int j = 0; j <= slices; ++j) {

            float U = (float)j / (float)slices;
            float theta = U * (PI * 2);

            // use spherical coordinates to calculate the positions.
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            positions.push_back(x * factor);
            positions.push_back(y * factor);
            positions.push_back(z * factor);
        }
    }

    // Calc The Index Positions
    for (int i = 0; i < slices * stacks + slices; ++i) {
        indices.push_back(GLuint(i));
        indices.push_back(GLuint(i + slices + 1));
        indices.push_back(GLuint(i + slices));

        indices.push_back(GLuint(i + slices + 1));
        indices.push_back(GLuint(i));
        indices.push_back(GLuint(i + 1));
    }

    sphereIndexCount = indices.size();

    // upload geometry to GPU.
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glGenBuffers(1, &spherePositionVbo);
    glBindBuffer(GL_ARRAY_BUFFER, spherePositionVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions.size(), positions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &sphereIndexVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereIndexVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
}

void Sphere::draw(Shader shader) const
{
    shader.use();
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);
}
