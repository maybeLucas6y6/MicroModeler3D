#version 330 core

out vec4 outColor;

in vec3 pos;

uniform vec4 col = vec4(1.0f);

void main()
{
	outColor = col * vec4(pos, 1.0f);
}