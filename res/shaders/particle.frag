#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D particleTexture;

void main()
{
	fragColor = texture(particleTexture, texCoords);
}