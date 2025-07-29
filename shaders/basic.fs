#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Color;

void main()
{
	//FragColor = vec4(0.5, 0.2, 0.3, 1.0);
	FragColor = vec4(Color, 1.0);
}
