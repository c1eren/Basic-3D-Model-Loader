#pragma once

class Mesh {
public:
	unsigned int m_meshId = 0;

	unsigned int indicesStart = 0;
	unsigned int indicesEnd = 0;
	// model[meshes[i]].indicesStart or something similar
};

/*
class AssimpModel : public StaticMesh3D
{
public:
	AssimpModel(const std::string& filePath, const std::string& defaultTextureName,
		bool withPositions, bool withTextureCoordinates, bool withNormals);
	AssimpModel(const std::string& filePath, bool withPositions = true, bool withTextureCoordinates = true, bool withNormals = true);
*/