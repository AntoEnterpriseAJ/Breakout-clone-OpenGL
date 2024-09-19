#version 430 core

in vec2 texPos;

out vec4 fragColor;

uniform sampler2D tex;

void main()
{
	fragColor = texture(tex, texPos);
}