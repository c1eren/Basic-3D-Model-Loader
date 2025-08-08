#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct Material {
	float shininess = 1.0f;
	glm::vec3 ambient = glm::vec3(0.2f);
	glm::vec3 diffuse = glm::vec3(1.0f);
};

struct TrackIndices {
	unsigned int indicesStart = 0;
	unsigned int indicesCount = 0;
	unsigned int baseVertex = 0;
};

class Mesh {
private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int texIds[3];

	TrackIndices tIndices;
	Material materials;
	
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
	
	void setTrackIndices(TrackIndices tInd) { tIndices = tInd; };

	std::vector<Vertex> getVertices() { return vertices; };
	std::vector<unsigned int> getIndices() { return indices; };
	unsigned int* getTexIds() { return &texIds[0]; };

	unsigned int getIndicesStart() { return tIndices.indicesStart; };
	unsigned int getIndicesCount() { return tIndices.indicesCount; };
	unsigned int getBaseVertex() { return tIndices.baseVertex; };
};