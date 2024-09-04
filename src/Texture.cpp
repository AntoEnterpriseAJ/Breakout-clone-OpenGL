#include "include/Texture.h"
#include <iostream>
#include "glad/glad.h"

#include "stb/stb_image.h"

Texture::Texture(const std::string& imagePath)
	: m_ID{0}
{
	stbi_set_flip_vertically_on_load(true);

	int width, height, channels;
	unsigned char* imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);

	if (imageData)
	{
		GLenum format;

		if (channels == 1)
			format = GL_RED;
		else if (channels == 3)
			format = GL_RGB;
		else if (channels == 4)
			format = GL_RGBA;
		else 
			std::cout << "WARNING: invalid image format" << std::endl;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
	{
		std::cout << "WARNING: couldn't load image data at path: " << imagePath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(imageData);
}

void Texture::bind() const
{
	glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
