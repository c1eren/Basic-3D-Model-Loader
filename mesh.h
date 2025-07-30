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
	
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
		: vertices(vertices), indices(indices) { };
	~Mesh() {}
};