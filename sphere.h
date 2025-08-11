#pragma once

#include <vector>
#include "shader.h"

const float PI = 3.14;

class Sphere {
public:
	Sphere(unsigned int stacks, unsigned int slices, float factor = 1.0);
	~Sphere() {};

private:
	std::vector<float> positions;
	std::vector<unsigned int> indices;

	unsigned int sphereVAO;
	unsigned int spherePositionVbo;
	unsigned int sphereIndexVbo;
	unsigned int sphereIndexCount;

public:
	void draw(Shader shader) const;
	std::vector<float>* getVertices();
	std::vector<unsigned int>* getIndices();
};