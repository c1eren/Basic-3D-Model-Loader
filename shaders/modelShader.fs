#version 460 core

#define NR_POINT_LIGHTS 1

out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;

struct Material {
	float shininess;
};
uniform Material materials;

struct LightSources {
	vec3 attenuation;
	vec3 position;
	vec3 color;
};
uniform LightSources lightSource[NR_POINT_LIGHTS];

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_normal;

void main()
{
	vec4 diffTex = texture(u_texture_diffuse, TexCoords);
	vec4 specTex = texture(u_texture_specular, TexCoords);
	vec4 normTex = texture(u_texture_normal, TexCoords);

	FragColor = diffTex;

	// TODO partial transparency support
	if (FragColor.a < 1.0)
	{
		discard;
	}
}
