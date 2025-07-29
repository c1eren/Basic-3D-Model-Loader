#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColors;

out vec3 FragPos;
out vec3 Color;

uniform mat4 model;

void main()
{
	Color = aColors;
	FragPos = aPos;
	gl_Position = model * vec4(aPos, 1.0);
}