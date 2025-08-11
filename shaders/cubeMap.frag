#version 460 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skybox;		// Note: samplerCube here instead of sampler2D

void main()
{
	FragColor = texture(skybox, TexCoords);
}
