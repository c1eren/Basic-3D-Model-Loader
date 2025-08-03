#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct TexIdType {
	unsigned int diff;
	unsigned int spec;
	unsigned int norm;
};


class Mesh {
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<TexIdType> textureIds;
	
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<TexIdType> textureIds)
		: vertices(vertices), indices(indices), textureIds(textureIds) { };
	~Mesh() {}
};