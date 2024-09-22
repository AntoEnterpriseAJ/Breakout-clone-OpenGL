#version 430 core

in vec2 texPos;

out vec4 fragColor;

uniform sampler2D tex;

uniform bool effectNormal		= false;
uniform bool effectInversion	= false;
uniform bool effectGrayscale	= false;

void main()
{
	if (effectNormal)
	{
		fragColor = texture(tex, texPos);
	}
	else if (effectInversion)
	{
		fragColor = vec4(1.0 - vec3(texture(tex, texPos)), 1.0f);
	}
	else if (effectGrayscale)
	{
		vec4 color = texture(tex, texPos);
		float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
		fragColor = vec4(vec3(average), 1.0f);
	}
}