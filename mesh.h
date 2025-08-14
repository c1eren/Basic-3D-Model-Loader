#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

struct TextureIds {
	unsigned int ti_diffuse = 0;
	unsigned int ti_specular = 0;
	unsigned int ti_normal = 0;
};

struct MaterialProperties {
	float shininess = 0.0f;
	float opacity	= 0.0f;
};

struct MaterialColors {
	glm::vec3 color_ambient		= glm::vec3(0.0f);
	glm::vec3 color_diffuse		= glm::vec3(0.0f);
	glm::vec3 color_specular	= glm::vec3(0.0f);
	glm::vec3 color_emissive	= glm::vec3(0.0f);
	glm::vec3 color_transparent = glm::vec3(0.0f);
};

struct TrackIndices {
	unsigned int indicesStart = 0;
	unsigned int indicesCount = 0;
	unsigned int baseVertex	  = 0;
};

class Mesh {
private:
	std::vector<Vertex>		  vertices;
	std::vector<unsigned int> indices;

	TrackIndices			  tIndices;
	MaterialColors			  mColors;
	MaterialProperties		  mProperties;
	
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, TextureIds ids, MaterialProperties mProps, MaterialColors mCols)
		: vertices(vertices), indices(indices), texIds(ids), mProperties(mProps), mColors(mCols) {}
	~Mesh() {}

	TextureIds				  texIds;

	
	// Setters
	void setTrackIndices(TrackIndices tInd) { tIndices = tInd; };

	// Getters
	std::vector<Vertex> getVertices() { return vertices; };
	std::vector<unsigned int> getIndices() { return indices; };
	TextureIds getTexIds() { return texIds; };

	unsigned int getIndicesStart() { return tIndices.indicesStart; };
	unsigned int getIndicesCount() { return tIndices.indicesCount; };
	unsigned int getBaseVertex() { return tIndices.baseVertex; };
	MaterialProperties getMaterialProps() { return mProperties; };
	MaterialColors getMaterialCols() { return mColors; };
};