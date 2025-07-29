#include "vbo.h"

#include <glad/glad.h>

Vbo::Vbo()
{
	glGenBuffers(1, &m_id);
}

void Vbo::addData(const void* data, unsigned int size)
{
	bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Vbo::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void Vbo::unbind()
{
	glDeleteBuffers(1, &m_id);
}

