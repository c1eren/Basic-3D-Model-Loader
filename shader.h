#pragma once

#include <glm/glm.hpp>


class Shader {
public:
	unsigned int m_programID;
	static unsigned int s_currentlyBoundId;
	static unsigned int s_shaderBindsPerLoop;

public:
	Shader() : m_programID(0) {}
	Shader(const char* vertexSourcePath, const char* fragmentSourcePath);
	Shader(const char* vertexSourcePath, const char* fragmentSourcePath, const char* geometrySourcePath);
	~Shader() {};

	//set texture int
	void setInt(const char* f_name, int val);

	//set float
	void setFloat(const char* f_name, float val);

	//set vec2
	void setVec2(const char* f_name, glm::vec2 vectors);

	//set vec3
	void setVec3(const char* f_name, glm::vec3 vectors);

	//set vec4
	void setVec4(const char* f_name, glm::vec4 vectors);

	//set matrix3
	void setMat3(const char* f_name, glm::mat3 matrix);

	//set matrix4
	void setMat4(const char* f_name, glm::mat4 matrix);

	void use();
};

