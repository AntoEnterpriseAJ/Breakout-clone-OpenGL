#version 430 core

layout (location = 0) in vec4 vertex;

out vec2 texCoords;

uniform float scale;
uniform vec2 offset;
uniform mat4 projection;

void main()
{
	texCoords = vertex.zw;

	gl_Position = projection * vec4(scale * vertex.xy + offset, 0.0f, 1.0f);
}