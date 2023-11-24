#version 330 core

out vec4 outColor;

in vec2 uvs;
in vec3 pos;

uniform sampler2D tex;

void main()
{
	outColor = texture(tex, uvs) * vec4(pos, 1.0f);
}