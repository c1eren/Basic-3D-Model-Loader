#include "vao.h"

#include <glad/glad.h>

Vao::Vao()
{
	glGenVertexArrays(1, &m_id);
}

void Vao::setLayout(bool positions, bool normal, bool texture, bool color, bool block)
{
	if (!block)
	{
		unsigned int stride = 0;
		unsigned int step = 0;

		if (positions)
			stride += 12; // 3 * sizeof(float)
		if (normal)
			stride += 12;
		if (texture)
			stride += 8;
		if (color)
			stride += 12;

		if (positions)
		{
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
			step += 12;
		}
		if (normal)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(step));
			step += 12;
		}
		if (texture)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(step));
			step += 8;
		}
		if (color)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(step));
		}
	}
	else
	{
		unsigned int step = 0;

		if (positions)
		{
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 9, GL_FLOAT, GL_FALSE, 0, (void*)0);
			step += 36;
		}
		if (normal)
		{
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 9, GL_FLOAT, GL_FALSE, 0, (void*)(step));
			step += 36;
		}
		if (texture)
		{
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 6, GL_FLOAT, GL_FALSE, 0, (void*)(step));
			step += 24;
		}
		if (color)
		{
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)(step));
		}
	}
}

/*
glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

glEnableVertexAttribArray(1);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

glEnableVertexAttribArray(2);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
*/

void Vao::bind()
{
	glBindVertexArray(m_id);
}

void Vao::unbind()
{
	glBindVertexArray(0);
}
