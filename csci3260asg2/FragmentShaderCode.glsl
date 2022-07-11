#version 430

struct Material {
	sampler2D myTexture;
	float shininess;
	int isLight;
	vec3 lightColor;
};

struct DirLight {
	vec3 direction;
	float intensity;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
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

in vec2 UV;
in vec3 normal;
in vec3 vertex;
in vec4 ShadowCoord;

uniform vec3 eyePosition;
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLight[3];
uniform SpotLight spotLight;
uniform sampler2DShadow shadowMap;
uniform vec3 lightInvDir;

out vec4 Color;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float visibility) {
	vec3 lightDir = normalize(-light.direction);
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Combine
	vec3 ambient = light.ambient * light.intensity;
	vec3 diffuse = light.diffuse * diff * light.intensity * visibility;
	vec3 specular = light.specular * spec * light.intensity * visibility;
	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 vertex, vec3 viewDir, float visibility) {
	vec3 lightDir = normalize(light.position - vertex);
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - vertex);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// Combine
	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * diff * attenuation * visibility;
	vec3 specular = light.specular * spec * attenuation * visibility;
	return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 vertex, vec3 viewDir, float visibility) {
	vec3 lightDir = normalize(light.position - vertex);
	// Diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Attenuation
	float distance = length(light.position - vertex);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	// Intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// Combine
	vec3 ambient = light.ambient * attenuation;
	vec3 diffuse = light.diffuse * diff * attenuation * intensity * visibility;
	vec3 specular = light.specular * spec * attenuation * intensity * visibility;
	return ambient + diffuse + specular;
}

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main()
{
	if (material.isLight)
		Color = vec4(material.lightColor, 1.0);
	else {
		// Visibility
		float cosTheta = clamp(dot(normal, lightInvDir), 0, 1);
		float bias = 0.005 * tan(acos(cosTheta));
		bias = clamp(bias, 0, 0.01);
		float visibility = 1.0;
		for (int i = 0; i < 4; i++) {
			int index = int(16.0 * random(floor(vertex * 1000.0), i)) % 16;
			visibility -= 0.2 * (1.0 - texture(shadowMap, vec3(ShadowCoord.xy + poissonDisk[index] / 700.0, (ShadowCoord.z - bias) / ShadowCoord.w)));
		}

		vec3 viewDir = normalize(eyePosition - vertex);

		// Directional light
		vec3 result = CalcDirLight(dirLight, normal, viewDir, visibility);
		// Point light
		for (int i = 0; i < 3; i++){
			result += CalcPointLight(pointLight[i], normal, vertex, viewDir, visibility);
		}
		// Spot light
		result += CalcSpotLight(spotLight, normal, vertex, viewDir, visibility);

		Color = vec4(result, 1.0) * texture(material.myTexture, UV);
	}
}