#version 460 core
out vec4 FragColor;

in vec3 FragPos;

void main()
{
	//FragColor = vec4(0.5, 0.2, 0.3, 1.0);
	FragColor = vec4(FragPos, 1.0);
}
