#version 430

struct Material {
	sampler2D myTexture;
	sampler2D myNormalTexture;
	float shininess;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragPos;
in vec2 UV;
in vec3 eyeWorldNormal;
in mat3 TBN;

uniform vec3 eyePosition;
uniform Material material;
uniform bool normalMappingFlag;
uniform PointLight pointLight;
uniform SpotLight spotLight;

out vec4 Color;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// Combine
	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * diff * attenuation;
	vec3 specular = light.specular * spec * attenuation;
	return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// Intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// Combine
	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * diff * attenuation * intensity;
	vec3 specular = light.specular * spec * attenuation * intensity;
	return ambient + diffuse + specular;
}

void main()
{
	vec3 normal = normalize(eyeWorldNormal);
	if(normalMappingFlag) {
		normal = texture(material.myNormalTexture, UV).rgb;
		normal = normal * 2.0 - 1.0;
		normal = normalize(TBN * normal);
	}

	vec3 viewDir = normalize(eyePosition - fragPos);

	// Point light
	vec3 result = CalcPointLight(pointLight, normal, fragPos, viewDir);

	// Spot light
	result += CalcSpotLight(spotLight, normal, fragPos, viewDir);

	Color = vec4(result, 1.0) * texture(material.myTexture, UV);
}