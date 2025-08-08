#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;
uniform int u_ObjectType; // 0 = cube, 1 = floor
uniform bool u_flashLight;

// FLOOR
struct floorMaterial {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
uniform floorMaterial floorMat;

// MATERIALS

struct Material {
	sampler2D texture_diffuse1; // Texture becomes our diffuse color
	sampler2D texture_specular1;
	//sampler2D emission;
	float shininess;
};
uniform Material material;

// DIRECTIONAL LIGHT

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

// POINT LIGHTS

struct PointLightProperties {
	vec3 pAmbient;
	vec3 pDiffuse;
	vec3 pSpecular;

	float attConstant;
	float attLinear;
	float attQuadratic;
};
uniform PointLightProperties pLProperties;

struct PointLight {
	vec3 position;
};
#define NR_POINT_LIGHTS 5
uniform PointLight pointLights[NR_POINT_LIGHTS]; // An array of pointLight structs

// SPOTLIGHT

struct FlashLight {
	vec3 position;
	vec3 direction;

	float flashAttConstant;
	float flashAttLinear;
	float flashAttQuadratic;

	vec3 flashAmbient;
	vec3 flashDiffuse;
	vec3 flashSpecular;

	float flashCutOff;
	float flashOuterCutOff;
};
uniform FlashLight flashLight;

// FUNCTIONS

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess);
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess);
vec3 CalcCube(vec3 norm, vec3 viewDir);
vec3 CalcSquare(vec3 norm, vec3 viewDir);

// So much optimisation to be done here

// MAIN

void main()
{
	// Properties
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);

	if (u_ObjectType == 0)
	{
		result = CalcCube(norm, viewDir);
	}
	else if (u_ObjectType == 1)
	{
		result = CalcSquare(norm, viewDir);
	}

	FragColor = vec4(result, 1.0);

}
vec3 CalcCube(vec3 norm, vec3 viewDir)
{
	vec3 texDiff = vec3(texture(material.diffuse, TexCoord));
	vec3 texSpec = vec3(texture(material.specular, TexCoord));
	vec3 result = vec3(0.0);

	// Phase 1: Directional lighting
	result = CalcDirLight(dirLight, norm, viewDir, texDiff, texSpec, material.shininess);

	// Phase 2: Point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texDiff, texSpec, material.shininess);
	}

	// Phase 3: Spot light
	if (u_flashLight)
	{
		result += CalcFlashLight(flashLight, norm, FragPos, viewDir, texDiff, texSpec, material.shininess);
	}

	return result;

}

vec3 CalcSquare(vec3 norm, vec3 viewDir)
{
	vec3 texDiff = vec3(texture(floorMat.diffuse, TexCoord));
	vec3 texSpec = vec3(texture(floorMat.specular, TexCoord));
	vec3 result = vec3(0.0);

	// Phase 1: Directional lighting
	result = CalcDirLight(dirLight, norm, viewDir, texDiff, texSpec, floorMat.shininess);

	// Phase 2: Point lights
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, texDiff, texSpec, floorMat.shininess);
	}

	// Phase 3: Spot light
	if (u_flashLight)
	{
		result += CalcFlashLight(flashLight, norm, FragPos, viewDir, texDiff, texSpec, floorMat.shininess);
	}

	return result;

}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess)
{
	// Light direction vector
	vec3 lightDir = normalize(-light.direction);

	// Diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

	// Results
	vec3 ambient = light.ambient * texDiff;
	vec3 diffuse = light.diffuse * diff * texDiff;
	vec3 specular = light.specular * spec * texSpec;

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess)
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
	float attenuation = 1.0 / (pLProperties.attConstant + pLProperties.attLinear * distance + pLProperties.attQuadratic * (distance * distance));

	// Results
	vec3 ambient = pLProperties.pAmbient * texDiff;
	vec3 diffuse = pLProperties.pDiffuse * diff * texDiff;
	vec3 specular = pLProperties.pSpecular * spec * texSpec;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texDiff, vec3 texSpec, float shininess)
{
	vec3 lightDir = normalize(light.position - fragPos);

	// Flashlight
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.flashCutOff - light.flashOuterCutOff;
	float intensity = clamp((theta - light.flashOuterCutOff) / epsilon, 0.0, 1.0);

	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.flashAttConstant + light.flashAttLinear * distance + light.flashAttQuadratic * (distance * distance));

	// Diffused
	float diff = max(dot(normal, lightDir), 0.0);

	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);


	vec3 ambient = light.flashAmbient * texDiff;
	vec3 diffuse = light.flashDiffuse * diff * texDiff;
	vec3 specular = light.flashSpecular * spec * texSpec;

	ambient *= attenuation;
	diffuse *= (intensity * attenuation);
	specular *= (intensity * attenuation);

	return (ambient + diffuse + specular);
}