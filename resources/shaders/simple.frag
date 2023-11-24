#version 330 core

out vec4 outColor;

uniform vec4 col = vec4(1.0f);

void main()
{
	outColor = col;
}