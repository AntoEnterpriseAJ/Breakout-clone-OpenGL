#include "Game.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <chrono>

static float deltaTime           = 0.0f;
static float lastTime            = 0.0f;
static glm::vec2 paddleSize      = {100.0f, 20.0f};
static glm::vec2 paddlePosition  = {static_cast<float>(800) / 2.0f - paddleSize.x / 2.0f, static_cast<float>(600) - paddleSize.y};
static float paddleVelocity      = 500.0f;

Game::Game(GLFWwindow* window,unsigned int width, unsigned int height)
    : m_window{window}, m_width{width}, m_height{height}, m_currentLevel{0}, m_state{GAME_ACTIVE}
{}

void Game::render()
{
    auto currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime  = currentTime;

    processInput();
    if (m_state == GAME_ACTIVE)
    {
        const Texture2D& background = ResourceManager::getTexture("background");
        m_spriteRenderer->drawSprite(background, {0.0f, 0.0f}, {m_width, m_height}, 0.0f);

        m_levels[m_currentLevel].draw(m_spriteRenderer);

        playerPaddle->draw(m_spriteRenderer);
    }
}

void Game::processInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);

    float velocity = paddleVelocity * deltaTime;

    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_LEFT == GLFW_PRESS))
    {
        playerPaddle->getPositionRef() += glm::vec2{-1.0f, 0.0f} * velocity;
    }
    else if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        playerPaddle->getPositionRef() += glm::vec2{1.0f, 0.0f} * velocity;
    }
}

void Game::init()
{
    ResourceManager::loadTexture("res/textures/awesomeface.png", "awesomeFace");
    ResourceManager::loadTexture("res/textures/background.jpg", "background");
    ResourceManager::loadTexture("res/textures/block_solid.png", "block_solid");
    ResourceManager::loadTexture("res/textures/block.png", "block");
    ResourceManager::loadTexture("res/textures/paddle.png", "paddle");

    ResourceManager::loadShader("res/shaders/sprite.vert", "res/shaders/sprite.frag", "spriteShader");

    GameLevel level1; level1.loadFromFile("res/levels/lvl1.txt", m_height / 2, m_width);
    GameLevel level2; level2.loadFromFile("res/levels/lvl2.txt", m_height / 2, m_width);
    GameLevel level3; level3.loadFromFile("res/levels/lvl3.txt", m_height / 2, m_width);
    GameLevel level4; level4.loadFromFile("res/levels/lvl4.txt", m_height / 2, m_width);

    m_levels.push_back(level1);
    m_levels.push_back(level2);
    m_levels.push_back(level3);
    m_levels.push_back(level4);

    playerPaddle = std::make_unique<GameObject>(ResourceManager::getTexture("paddle"), paddlePosition,
                                                paddleSize, glm::vec3{1.0f, 1.0f, 1.0f}, true);

    m_spriteRenderer = std::make_unique<SpriteRenderer>(ResourceManager::getShader("spriteShader"));

    Shader& shader = ResourceManager::getShader("spriteShader");

    shader.use();

    auto orthographicProjection = [](float l, float r, float b, float t, float n, float f){
        glm::mat4 ortho = {
            2.0f / (r - l)  , 0.0f           , 0.0f             , - (r + l) / (r - l),
            0.0f            , 2.0f / (t - b) , 0.0f             , - (t + b) / (t - b),
            0.0f            , 0.0f           , - 2.0f / (f - n) , - (f + n) / (f - n),
            0.0f            , 0.0f           , 0.0f             , 1.0f,
        };

        return glm::transpose(ortho);
    };

    glm::mat4 projection = orthographicProjection(0.0f, static_cast<float>(m_width), 
                                                  static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);

    shader.setInt("spriteTexture", 0);
    shader.setMat4("projection", projection);
}
