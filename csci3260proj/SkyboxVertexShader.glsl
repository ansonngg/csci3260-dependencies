#version 430

in layout(location = 0) vec3 position;

uniform mat4 skyboxMatrix;

out vec3 texCoords;

void main()
{
	texCoords = position;
	gl_Position = skyboxMatrix * vec4(position, 1.0);
}