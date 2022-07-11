#version 430

in vec3 texCoords;

uniform samplerCube skybox;

out vec4 Color;

void main()
{
	Color = texture(skybox, texCoords);
}