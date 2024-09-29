#include "TextRenderer.h"
#include "glad/glad.h"
#include <iostream>

TextRenderer::TextRenderer(const char* fontPath, unsigned int fontSize)
{
    if (FT_Init_FreeType(&m_FT))
    {
        std::cout << "ERROR::FREETYPE: failed to init FreeType" << std::endl;
        exit(-1);
    }

    if (FT_New_Face(m_FT, fontPath, 0, &m_Face))
    {
        std::cout << "ERROR::FREETYPE: failed to load font at path: " << fontPath << std::endl;
        exit(-1);
    }

    FT_Set_Pixel_Sizes(m_Face, 0, fontSize);  
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char character = 0; character < 128; ++character)
    {
        if(FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYPE: failed to load Glyph" << std::endl;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            m_Face->glyph->bitmap.width,
            m_Face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            m_Face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        m_characterData[character] = CharacterData{
            texture,
            static_cast<unsigned int>(m_Face->glyph->advance.x),
            glm::ivec2{m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows},
            glm::ivec2{m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top},
        };

        FT_Done_Face(m_Face);
        FT_Done_FreeType(m_FT);
    }
}

TextRenderer::~TextRenderer()
{
    for (const auto& [character, data]: m_characterData)
    {
        glDeleteTextures(1, &data.texture);
    }
}

void TextRenderer::RenderText(const Shader& shader, const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    float charX = x;
    float charY = y;

    for (const auto& character : text)
    {
        const CharacterData& charData = m_characterData[character];

        glm::mat4 model{1.0f};

    }
}
