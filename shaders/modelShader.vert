#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_FS {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} to_fs;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
	vec4 worldPos = model * vec4(aPos, 1.0);

	to_fs.Normal	= aNormal * normalMatrix;
	to_fs.TexCoords = aTexCoords;
	to_fs.FragPos	= vec3(worldPos);

	gl_Position		= projection * view * worldPos;
}