#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform float fadeFactor;
uniform sampler2D particleTexture;

void main()
{
	fragColor = fadeFactor * texture(particleTexture, texCoords);
}