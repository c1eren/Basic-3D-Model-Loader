#version 460 core
out vec4 FragColor;

in vec4 FragPos;
in vec2 TexCoords;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_normal;

uniform int toggle; // 0: color, 1: tex

void main()
{
	//FragColor = vec4(0.4, 0.5, 0.6, 1.0);
	FragColor = texture(u_texture_diffuse, TexCoords);
	//if (toggle == 0)
	//	FragColor = vec4(0.4, 0.5, 0.6, 1.0);
	//else if (toggle == 1)
	//	FragColor = texture(u_tex, TexCoords);
	//else 
	//	FragColor = vec4(1.0);
}
