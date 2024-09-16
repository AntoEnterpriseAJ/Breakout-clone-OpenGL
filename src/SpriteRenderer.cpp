#include "SpriteRenderer.h"
#include "glad/glad.h"

SpriteRenderer::SpriteRenderer(Shader& shader)    
    : m_quadVAO{0}, m_shader{shader}
{
    initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &m_quadVAO);
}

void SpriteRenderer::drawSprite(const Texture2D& texture, const glm::vec2& position, const glm::vec2& size,
                                float rotateAngle, const glm::vec3& color) const
{
    m_shader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    m_shader.setMat4("model", model);

    m_shader.setInt("spriteTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getID());

    m_shader.setVec3("spriteColor", color);

    glBindVertexArray(m_quadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
}

void SpriteRenderer::initRenderData()
{
    unsigned int quadVBO, quadEBO;

    float vertices[] = { 
    // pos          // tex
    0.0f, 0.0f,     0.0f, 0.0f, 
    1.0f, 0.0f,     1.0f, 0.0f,
    0.0f, 1.0f,     0.0f, 1.0f,
    1.0f, 1.0f,     1.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 1 
    };

    glGenVertexArrays(1, &m_quadVAO);
    glBindVertexArray(m_quadVAO);

    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &quadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
}
