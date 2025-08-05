#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int texIds[3];
	unsigned int indicesStart = 0;
	unsigned int indicesCount = 0;
	unsigned int baseVertex	  = 0;
	
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int ids[3])
		: vertices(vertices), indices(indices) 
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			texIds[i] = ids[i];
		}
	}
	~Mesh() {}
};