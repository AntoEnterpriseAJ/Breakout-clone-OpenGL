#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexPos;

uniform bool effectShake          = false;
uniform bool effectConfuse        = false;
uniform bool effectChaos          = false;

uniform float time;

out vec2 texPos;

void main()
{
	texPos = aTexPos;
	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);

	if (effectConfuse)
	{
		gl_Position = vec4(aPos.x, -aPos.y, 0.0f, 1.0f);	
	}
	else if (effectChaos)
	{
		float strength = 0.05;
		float radius   = 0.5f;
		texPos.x += radius * cos(time);
		texPos.y += radius * sin(time);
	}

	if (effectShake)
	{
		float strength = 0.005f;
		gl_Position.x += cos(time * 20) * strength;
		gl_Position.y += sin(time * 20) * strength;
	}

	return;
}