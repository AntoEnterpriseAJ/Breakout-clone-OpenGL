#version 430 core

in vec2 texPos;

out vec4 fragColor;

uniform sampler2D tex;

uniform bool effectNormal	 = false;
uniform bool effectInversion = false;
uniform bool effectGrayscale = false;
uniform bool effectBlur		 = false;
uniform bool effectSharpen	 = false;

// TODO: add screenHeight and screenWidth;
const float xOffset = 1.0 / 800.0;
const float yOffset = 1.0 / 600.0;

const float kernelBlur[9] = {
	1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
	1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
	1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0,
};

const float kernelSharpen[9] = {
	-1.0, -1.0, -1.0,
	-1.0,  9.0, -1.0,
	-1.0, -1.0, -1.0,
};

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

	const vec2 offsets[] = {
		vec2(-xOffset, -yOffset), vec2(0, -yOffset), vec2(xOffset, -yOffset),  // TOP
		vec2(-xOffset, 0)		, vec2(0, 0)	   , vec2(xOffset, 0)	    ,  // MIDDLE
		vec2(-xOffset, yOffset) , vec2(0, yOffset) , vec2(xOffset, yOffset) ,  // BOTTOM
	};

	float kernel[9];
	vec3 pixels[9];
	for (int i = 0; i < 9; ++i)
	{
		pixels[i] = vec3(texture(tex, texPos.xy + offsets[i]));
	}

	if		(effectBlur)	kernel = kernelBlur;
	else if (effectSharpen) kernel = kernelSharpen;

	vec3 color = vec3(0.0);
	for (int i = 0; i < 9; ++i)
	{
		color += pixels[i] * kernel[i];
	}

	fragColor = vec4(color, 1.0f);
}