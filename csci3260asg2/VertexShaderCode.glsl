#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 vertexUV;
in layout(location = 2) vec3 vertexNormal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 depthBiasMVP;

out vec2 UV;
out vec3 normal;
out vec3 vertex;
out vec4 ShadowCoord;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 newPosition = modelMatrix * v;
	gl_Position = projectionMatrix * viewMatrix * newPosition;
	ShadowCoord = depthBiasMVP * newPosition;
	vec4 normal_temp = modelMatrix * vec4(vertexNormal, 0);
	normal = normalize(normal_temp.xyz);
	vertex = newPosition.xyz;
	UV = vertexUV;
}
