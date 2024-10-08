#version 430 core

in vec2 texCoords;

out vec4 fragColor;

uniform sampler2D spriteTexture;
uniform vec3 spriteColor;

void main()
{
	fragColor = vec4(spriteColor, 1.0) * texture(spriteTexture, texCoords);
}