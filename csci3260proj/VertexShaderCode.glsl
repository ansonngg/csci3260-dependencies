#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 vertexUV;
in layout(location = 2) vec3 vertexNormal;
in layout(location = 3) vec3 tangent;
in layout(location = 4) vec3 bitangent;

uniform mat4 modelMatrix;
uniform mat4 eyeWorldMatrix;

out vec3 fragPos;
out vec2 UV;
out vec3 eyeWorldNormal;
out mat3 TBN;

void main()
{
	vec4 newPosition = modelMatrix * vec4(position, 1.0);
	gl_Position = eyeWorldMatrix * newPosition;
	fragPos = newPosition.xyz;
	UV = vertexUV;
	eyeWorldNormal = (modelMatrix * vec4(vertexNormal, 0.0)).xyz;
	vec3 T = normalize((modelMatrix * vec4(tangent, 0.0)).xyz);
	vec3 B = normalize((modelMatrix * vec4(bitangent, 0.0)).xyz);
	vec3 N = normalize((modelMatrix * vec4(vertexNormal, 0.0)).xyz);
	TBN = mat3(T, B, N);
}