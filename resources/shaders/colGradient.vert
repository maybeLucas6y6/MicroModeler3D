#version 330 core

layout (location = 0) in vec3 position;

out vec3 pos;
					
uniform mat4 proj = mat4(1.0f);
uniform mat4 view = mat4(1.0f);
uniform mat4 model = mat4(1.0f);

void main()
{
	pos = position;
	gl_Position = proj * view * model * vec4(position, 1.0f);
}