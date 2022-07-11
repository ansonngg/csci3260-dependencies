#version 430

in layout(location = 0) vec3 position;

uniform mat4 modelMatrix;
uniform mat4 depthMVP;

void main()
{
	gl_Position = depthMVP * modelMatrix * vec4(position, 1.0);
}