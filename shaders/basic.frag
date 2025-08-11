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
	vec4 diffTex = texture(u_texture_diffuse, TexCoords);
	vec4 specTex = texture(u_texture_specular, TexCoords);
	vec4 normTex = texture(u_texture_normal, TexCoords);

	FragColor = diffTex;
	if (FragColor.a < 1.0)
	{
		discard;
	}
}
