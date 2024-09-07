#pragma once
#include "Texture2D.h"
#include "Shader.h"
#include "glm/glm.hpp"

class SpriteRenderer
{
public:
    SpriteRenderer(Shader& shader);
    ~SpriteRenderer();

    void drawSprite(const Texture2D& texture, const glm::vec2& position, const glm::vec2& size,
                    float rotateAngle, const glm::vec3& color);

private:
    void initRenderData();
private:
    unsigned int m_quadVAO;
    Shader& m_shader;
};
