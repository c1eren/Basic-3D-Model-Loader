#version 460 core
#define NR_POINT_LIGHTS 14
out vec4 FragColor;

in VS_FS {
	vec2 TexCoords;
	vec3 Normal;
	vec3 FragPos;
} from_vs;

struct MaterialProperties {
	float shininess;
	float opacity;
};
uniform MaterialProperties u_matProps;

struct MaterialColors {
	vec3 ambient;		
	vec3 diffuse;		
	vec3 specular;	
	vec3 emissive;	
	vec3 transparent;
};
uniform MaterialColors u_matCols;

struct LightSource {
	vec3 attenuation;
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform LightSource u_lightSource[NR_POINT_LIGHTS];

uniform vec3 u_cameraViewPos;

uniform sampler2D u_texture_diffuse;
uniform sampler2D u_texture_specular;
uniform sampler2D u_texture_normal;

vec3 calcPointLight(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex, vec3 specTex, float shininess);

void main()
{
	vec3 norm = normalize(from_vs.Normal);
	vec3 viewDir = normalize(u_cameraViewPos - from_vs.FragPos);

	vec3 diffTex = vec3(texture(u_texture_diffuse, from_vs.TexCoords));
	vec3 specTex = vec3(texture(u_texture_specular, from_vs.TexCoords));
	vec3 normTex = vec3(texture(u_texture_normal, from_vs.TexCoords));

	vec3 final = vec3(0.0); // Check transparency if shader fails

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		final += calcPointLight(u_lightSource[i], norm, from_vs.FragPos, viewDir, diffTex, specTex, u_matProps.shininess);
	}

	FragColor = vec4(final, 1.0);

	// TODO partial transparency support
	if (FragColor.a < 1.0)
	{
		discard;
	}
}

vec3 calcPointLight(LightSource light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 diffTex, vec3 specTex, float shininess)
{
	// Light direction vector 
	vec3 lightDir = normalize(light.position - fragPos);

	// Diffuse shading 
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * (distance * distance));

	// Results
	vec3 ambient = light.ambient * diffTex;
	vec3 diffuse = light.diffuse * diff * diffTex;
	vec3 specular = light.specular * spec * specTex;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}