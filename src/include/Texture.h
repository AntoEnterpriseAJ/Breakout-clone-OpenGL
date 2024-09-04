#pragma once
#include <string>

class Texture
{
public:
	Texture(const std::string& imagePath);

	void bind() const;
	void unbind() const;
private:
	unsigned int m_ID;
};