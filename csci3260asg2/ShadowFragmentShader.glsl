#version 430

out layout(location = 0) float fragmentdepth;

void main()
{
	fragmentdepth = gl_FragCoord.z;
}