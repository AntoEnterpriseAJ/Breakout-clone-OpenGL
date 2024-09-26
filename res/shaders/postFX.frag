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
    vec4 baseColor = texture(tex, texPos);
    vec3 pixels[9];

    const vec2 offsets[] = {
        vec2(-xOffset, -yOffset), vec2(0, -yOffset), vec2(xOffset, -yOffset)  ,  // TOP
        vec2(-xOffset, 0)        , vec2(0, 0)       , vec2(xOffset, 0)        ,  // MIDDLE
        vec2(-xOffset, yOffset)  , vec2(0, yOffset) , vec2(xOffset, yOffset)  ,  // BOTTOM
    };

    if (effectChaos || effectShake)
    {
        for (int i = 0; i < 9; i++)
        {
            pixels[i] = vec3(texture(tex, texPos.xy + offsets[i]));
        }
    }

    if (effectChaos)
    {
        for (int i = 0; i < 9; i++)
        {
            fragColor += vec4(pixels[i] * kernelEdgeDetection[i], 0.0f);
        }
		fragColor.a = 1.0f;
		return;
    }

    if (effectShake)
    {
        for (int i = 0; i < 9; i++)
        {
            fragColor += vec4(pixels[i] * kernelBlur[i], 0.0f);
        }
    }

    if (fragColor == vec4(0.0f))
    {
        fragColor = baseColor;
    }

    if (effectConfuse)
    {
        invertColors(fragColor.rgb);
    }

    fragColor.a = 1.0f;
}

