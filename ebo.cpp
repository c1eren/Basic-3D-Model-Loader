#include "ebo.h"

#include <glad/glad.h>

Ebo::Ebo()
{
	glGenBuffers(1, &m_id);
}

Ebo::~Ebo()
{
	unbind();
}

void Ebo::addData(const void* data, unsigned int size)
{
	bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Ebo::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void Ebo::unbind()
{
	glDeleteBuffers(1, &m_id);
}