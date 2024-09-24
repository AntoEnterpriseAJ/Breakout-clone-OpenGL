#version 430 core

in vec2 texPos;

out vec4 fragColor;

uniform sampler2D tex;

uniform bool effectShake          = false;
uniform bool effectConfuse        = false;
uniform bool effectChaos          = false;

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

const float kernelEdgeDetection[9] = {
	 0.0,  1.0,  0.0,
	 1.0, -4.0,  1.0,
	 0.0,  1.0,  0.0,
};

void invertColors(inout vec3 color)
{
	color = vec3(1.0 - color);
}

void grayScale(inout vec3 color)
{
	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	color = vec3(average);
}

void main()
{
	vec4 color = texture(tex, texPos);

	if (effectShake || effectConfuse || effectChaos)
	{
		const vec2 offsets[] = {
			vec2(-xOffset, -yOffset), vec2(0, -yOffset), vec2(xOffset, -yOffset),  // TOP
			vec2(-xOffset, 0)		, vec2(0, 0)	   , vec2(xOffset, 0)	    ,  // MIDDLE
			vec2(-xOffset, yOffset) , vec2(0, yOffset) , vec2(xOffset, yOffset) ,  // BOTTOM
		};

		float kernel[9];
		vec3  pixels[9];
		for (int i = 0; i < 9; ++i)
		{
			pixels[i] = vec3(texture(tex, texPos.xy + offsets[i]));
		}

		if (effectShake)	    kernel = kernelBlur;
		else if (effectConfuse) kernel = kernelBlur;
		else if (effectChaos)	kernel = kernelEdgeDetection;

		vec3 color = vec3(0.0);
		for (int i = 0; i < 9; ++i)
		{
			color += pixels[i] * kernel[i];
		}

		if (effectConfuse)
		{
			invertColors(color);
		}

		fragColor = vec4(color, 1.0f);
		return;
	}

	fragColor = color;

	return;
}