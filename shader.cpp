#include "shader.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

unsigned int Shader::s_currentlyBoundId = 0;

Shader::Shader(const char* vertexSourcePath, const char* fragmentSourcePath)
{
	std::string vString;
	std::string fString;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open files
		vShaderFile.open(vertexSourcePath);
		fShaderFile.open(fragmentSourcePath);

		// StringStream objects
		std::stringstream vShaderStream;
		std::stringstream fShaderStream;

		// Read file contents into streams 
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close files (redundant)
		vShaderFile.close();
		fShaderFile.close();

		// Convert StringStream contents to c++ string
		vString = vShaderStream.str();
		fString = fShaderStream.str();
	}
	catch(std::ifstream::failure &e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << e.what() << std::endl;
	}

	// Convert to c_strings
	const char* vertCode = vString.c_str();
	const char* fragCode = fString.c_str();

	// Create shaders
	unsigned int vertex;
	unsigned int fragment;

	int success;
	char infoLog[512];

	// fragment shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Shader program
	this->m_programID = glCreateProgram();
	glAttachShader(this->m_programID, vertex);
	glAttachShader(this->m_programID, fragment);
	glLinkProgram(this->m_programID);

	glGetProgramiv(this->m_programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->m_programID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

// Geometry shader overloaded constructor
Shader::Shader(const char* vertexSourcePath, const char* fragmentSourcePath, const char* geometrySourcePath)
{
	std::string vString;
	std::string fString;
	std::string gString;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// Open files
		vShaderFile.open(vertexSourcePath);
		fShaderFile.open(fragmentSourcePath);
		gShaderFile.open(geometrySourcePath);

		// StringStream objects
		std::stringstream vShaderStream;
		std::stringstream fShaderStream;
		std::stringstream gShaderStream;

		// Read file contents into streams 
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();

		// Close files (redundant)
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();

		// Convert StringStream contents to c++ string
		vString = vShaderStream.str();
		fString = fShaderStream.str();
		gString = gShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << e.what() << std::endl;
	}

	// Convert to c_strings
	const char* vertCode = vString.c_str();
	const char* fragCode = fString.c_str();
	const char* geomCode = gString.c_str();

	// Create shaders
	unsigned int vertex;
	unsigned int fragment;
	unsigned int geometry;

	int success;
	char infoLog[512];

	// Vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Geometry shader
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &geomCode, NULL);
	glCompileShader(geometry);

	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(geometry, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Shader program
	this->m_programID = glCreateProgram();
	glAttachShader(this->m_programID, vertex);
	glAttachShader(this->m_programID, fragment);
	glAttachShader(this->m_programID, geometry);
	glLinkProgram(this->m_programID);

	glGetProgramiv(this->m_programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->m_programID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

void Shader::setInt(const char* f_name, int val)
{
	use();
	glUniform1i(glGetUniformLocation(this->m_programID, f_name), val);
}

void Shader::setFloat(const char* f_name, float val)
{
	use();
	glUniform1f(glGetUniformLocation(this->m_programID, f_name), val);
}

void Shader::setVec2(const char* f_name, glm::vec2 vectors)
{
	use();
	glUniform3fv(glGetUniformLocation(this->m_programID, f_name), 1, glm::value_ptr(vectors));
}

void Shader::setVec3(const char* f_name, glm::vec3 vectors)
{
	use();
	glUniform3fv(glGetUniformLocation(this->m_programID, f_name), 1, glm::value_ptr(vectors));
}

void Shader::setVec4(const char* f_name, glm::vec4 vectors)
{
	use();
	glUniform4fv(glGetUniformLocation(this->m_programID, f_name), 1, glm::value_ptr(vectors));
}

void Shader::setMat3(const char* f_name, glm::mat3 matrix)
{
	use(); // Don't forget to merge this
	glUniformMatrix3fv(glGetUniformLocation(this->m_programID, f_name), 1, GL_FALSE, glm::value_ptr(matrix));

}

void Shader::setMat4(const char* f_name, glm::mat4 matrix)
{
	use(); // Don't forget to merge this
	glUniformMatrix4fv(glGetUniformLocation(this->m_programID, f_name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::use()
{
	if (m_programID != Shader::s_currentlyBoundId)
	{
		glUseProgram(m_programID);
		Shader::s_currentlyBoundId = m_programID;
	}
}
