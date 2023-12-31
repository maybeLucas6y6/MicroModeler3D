#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoords;

out vec2 uvs;
out vec3 pos;
					
uniform mat4 proj = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

void main()
{
	uvs = texcoords;
	pos = position;
	gl_Position = proj * view * model * vec4(position, 1.0);
}