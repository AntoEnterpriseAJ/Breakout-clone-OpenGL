#include "Game.h"
#include "ResourceManager.h"
#include "glm/glm.hpp"

Game::Game(unsigned int width, unsigned int height)
    : m_width{width}, m_height{height}
{
    init();
}

void Game::render()
{
    m_spriteRenderer->drawSprite(ResourceManager::getTexture("awesomeFace"), glm::vec2(200.0f, 200.0f),
                                 glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Game::init()
{
    ResourceManager::loadTexture("res/textures/awesomeface.png", "awesomeFace");
    ResourceManager::loadShader("res/shaders/sprite.vert", "res/shaders/sprite.frag", "spriteShader");

    m_spriteRenderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("spriteShader"));

    Shader& shader = ResourceManager::getShader("spriteShader");

    shader.use();

    auto orthographicProjection = [](float l, float r, float b, float t, float n, float f){
        glm::mat4 ortho = {
            2.0f / (r - l)  , 0.0f           , 0.0f             , - (l + r) / (r - l),
            0.0f            , 2.0f / (t - b) , 0.0f             , - (t + b) / (t - b),
            0.0f            , 0.0f           , 2.0f / (f - n)   ,   (f + n) / (f - n),
            0.0f            , 0.0f           , 0.0f             , 1.0f,
        };

        return glm::transpose(ortho);
    };

    glm::mat4 projection = orthographicProjection(0.0f, static_cast<float>(m_width), 
                                      static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

    shader.setInt("spriteTexture", 0);
    shader.setMat4("projection", projection);
}
