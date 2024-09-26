#include "Game.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "GameConstants.h"

// TODO: fix useless static variables
static float deltaTime           = 0.0f;
static float lastTime            = 0.0f;

static float shakeTime;

static std::unique_ptr<BallObject>          ball;
static std::unique_ptr<ParticleGenerator>   particleGenerator;
static std::unique_ptr<PostProcessor>       postProcessor; 

Game::Game(GLFWwindow* window,unsigned int width, unsigned int height)
    : m_window{window}, m_width{width}, m_height{height}, m_currentLevel{0}, m_state{GAME_ACTIVE}
{
    init();
}

void Game::render()
{
    if (m_state == GAME_ACTIVE)
    {
        postProcessor->beginOffscreenRendering();

        m_spriteRenderer->drawSprite(ResourceManager::getTexture("background"), {0.0f, 0.0f}, {m_width, m_height}, 0.0f);
        m_levels[m_currentLevel].draw(m_spriteRenderer);
        playerPaddle->draw(m_spriteRenderer);

        for (auto& powerUp : m_powerUps)
        {
            if (!powerUp.isDestroyed())
            {
                powerUp.draw(m_spriteRenderer);
            }
        }

        particleGenerator->render(ResourceManager::getShader("particleShader"));
        ball->draw(m_spriteRenderer);

        postProcessor->endOffscreenRendering();
        postProcessor->render();
    }
}

void Game::update()
{
    float currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime  = currentTime;

    processInput();
    ball->move(deltaTime, m_width);
    handleCollisions();
    particleGenerator->update(*ball, deltaTime);
    updatePowerUps();

    shakeTime -= deltaTime;
    if (shakeTime < 0)
    {
        postProcessor->stopEffect("effectShake");
    }
}

void Game::processInput()
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);

    float paddleVelocity = GameConstants::initPadSpeed * deltaTime;

    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        playerPaddle->getPositionRef().x -= paddleVelocity;
        if (ball->onPaddle())
        {
            ball->getPositionRef().x -= paddleVelocity;
        }
    }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        playerPaddle->getPositionRef().x += paddleVelocity;
        if (ball->onPaddle())
        {
            ball->getPositionRef().x += paddleVelocity;
        }
    }
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        ball->leavePaddle();
    }
}

void Game::trySpawnPowerUp(const GameObject& brick)
{
    if (rand() % 100 < 99)
    {
        glm::vec2 size = {50.0f, 20.0f};
        glm::vec2 spawnPoint = brick.getPosition() + glm::vec2{brick.getSize().x, brick.getSize().y} / 2.0f - size / 2.0f;

        for (int i = 0; i < m_powerUps.size(); ++i)
        {
            if (!m_powerUps[i].isDestroyed())
                continue; 

            PowerUp::Type effect = (PowerUp::Type)(rand() % (int)PowerUp::Type::count);
            float duration       = 0.0f;
            std::string texture  = "";

            if (effect == PowerUp::Type::speedUp)
            {
                duration = GameConstants::effectSpeedUpTime;
                texture = "powerup_speed";
            }
            else if (effect == PowerUp::Type::confuse)
            {
                duration = GameConstants::effectConfuseTime;
                texture = "powerup_confuse";
            }
            else if (effect == PowerUp::Type::chaos)
            {
                duration = GameConstants::effectChaosTime;
                texture = "powerup_chaos";
            }

            m_powerUps[i] = PowerUp{ResourceManager::getTexture(texture), spawnPoint, size, effect, duration};
            return;
        }
    }
}

void Game::updatePowerUps()
{
    for (auto& powerUp : m_powerUps)
    {
        if (!powerUp.isDestroyed())
            powerUp.updateMove(deltaTime, m_height);

        if (powerUp.isActive())
        {
            powerUp.updatEffectDuration(deltaTime);

            if(!powerUp.isActive())
            {
                if (powerUp.getType() == PowerUp::Type::speedUp)
                {
                    ball->getVelocityRef() /= 1.15f;
                }
                else if (powerUp.getType() == PowerUp::Type::confuse)
                {
                    postProcessor->stopEffect("effectConfuse");
                }
                else if (powerUp.getType() == PowerUp::Type::chaos)
                {
                    postProcessor->stopEffect("effectChaos");
                }
            }
        }
    }
}

void Game::activatePowerUp(PowerUp& powerUp)
{
    powerUp.activate();

    if (powerUp.getType() == PowerUp::Type::speedUp)
    {
        ball->getVelocityRef() *= 1.15f;
    }
    else if (powerUp.getType() == PowerUp::Type::confuse)
    {
        postProcessor->startEffect("effectConfuse");
    }
    else if (powerUp.getType() == PowerUp::Type::chaos)
    {
        postProcessor->startEffect("effectChaos");
    }
}

void Game::handleCollisions()
{
    using Direction = CollisionManager::Direction;
    GameLevel& currentLevel = m_levels[m_currentLevel];

    for (auto& brick : currentLevel.getBricks())
    {
        if (brick.isDestroyed())
            continue;

        auto [brickCollision, direction, offset] = m_collisionManager.getCollisionStatus(*ball, brick);

        if (brickCollision)
        {
            ball->getPositionRef() += offset;   // get the ball out of the object it collides with

            if (!brick.isBreakable())
            {
                postProcessor->startEffect("effectShake");
                shakeTime = GameConstants::shakeTime;
            }

            if (brick.isBreakable())
            {
                trySpawnPowerUp(brick);
                brick.destroy();
            }

            if (direction == Direction::LEFT || direction == Direction::RIGHT)
            {
                ball->getVelocityRef().x = -ball->getVelocityRef().x;
            }
            else if (direction == Direction::UP || direction == Direction::DOWN)
            {
                ball->getVelocityRef().y = -ball->getVelocityRef().y;
            }

            break;
        }
    }

    if (!ball->onPaddle())
    {
        auto [paddleCollision, direction, offset] = m_collisionManager.getCollisionStatus(*ball, *playerPaddle);

        if (paddleCollision)
        {
            glm::vec2 paddleCenter = playerPaddle->getPosition() + playerPaddle->getSize() / 2.0f;
            glm::vec2 ballCenter   = ball->getPosition() + ball->getSize() / 2.0f;

            glm::vec2 bounceDirection = glm::normalize(ballCenter - paddleCenter);

            ball->getVelocityRef() = bounceDirection * glm::length(ball->getVelocity());
        }
    }

    for (auto& powerUp : m_powerUps)
    {
        if (powerUp.isDestroyed())
            continue;

        bool collision = m_collisionManager.getCollisionStatus(*playerPaddle, powerUp);
        if (collision)
        {
            activatePowerUp(powerUp);
            powerUp.destroy();
        }
    }
}

void Game::init()
{
    ResourceManager::loadTexture("res/textures/awesomeface.png", "ball");
    ResourceManager::loadTexture("res/textures/background.jpg", "background");
    ResourceManager::loadTexture("res/textures/block_solid.png", "block_solid");
    ResourceManager::loadTexture("res/textures/block.png", "block");
    ResourceManager::loadTexture("res/textures/paddle.png", "paddle");
    ResourceManager::loadTexture("res/textures/particle.png", "particle");
    ResourceManager::loadTexture("res/textures/powerup_speed.png", "powerup_speed");
    ResourceManager::loadTexture("res/textures/powerup_confuse.png", "powerup_confuse");
    ResourceManager::loadTexture("res/textures/powerup_chaos.png", "powerup_chaos");

    ResourceManager::loadShader("res/shaders/sprite.vert", "res/shaders/sprite.frag", "spriteShader");
    ResourceManager::loadShader("res/shaders/particle.vert", "res/shaders/particle.frag", "particleShader");
    ResourceManager::loadShader("res/shaders/postFX.vert", "res/shaders/postFX.frag", "postFX");

    GameLevel level1; level1.loadFromFile("res/levels/lvl1.txt", m_height / 2, m_width);
    GameLevel level2; level2.loadFromFile("res/levels/lvl2.txt", m_height / 2, m_width);
    GameLevel level3; level3.loadFromFile("res/levels/lvl3.txt", m_height / 2, m_width);
    GameLevel level4; level4.loadFromFile("res/levels/lvl4.txt", m_height / 2, m_width);

    m_levels.push_back(level1);
    m_levels.push_back(level2);
    m_levels.push_back(level3);
    m_levels.push_back(level4);

    ParticleGenerator::Particle particle{ResourceManager::getTexture("particle"), 15.0f, 1.0f, 0.5};

    playerPaddle        = std::make_unique<GameObject>(ResourceManager::getTexture("paddle"), GameConstants::initPadPos,
                                                       GameConstants::initPadSize);
    ball                = std::make_unique<BallObject>(ResourceManager::getTexture("ball"), GameConstants::initBallPos,
                                                       GameConstants::ballInitVel, GameConstants::initBallRadius);
    m_spriteRenderer    = std::make_unique<SpriteRenderer>(ResourceManager::getShader("spriteShader"));
    particleGenerator   = std::make_unique<ParticleGenerator>(particle, 100);
    postProcessor       = std::make_unique<PostProcessor>(ResourceManager::getShader("postFX"), m_width, m_height);

    auto orthographicProjection = [](float l, float r, float b, float t, float n, float f){
        glm::mat4 ortho = {
            2.0f / (r - l)  , 0.0f           , 0.0f             , - (r + l) / (r - l),
            0.0f            , 2.0f / (t - b) , 0.0f             , - (t + b) / (t - b),
            0.0f            , 0.0f           , - 2.0f / (f - n) , - (f + n) / (f - n),
            0.0f            , 0.0f           , 0.0f             , 1.0f,
        };

        return glm::transpose(ortho);
    };

    glm::mat4 projection = orthographicProjection(0.0f, (float)m_width, (float)m_height, 0.0f, -1.0f, 1.0f);

    Shader& spriteShader = ResourceManager::getShader("spriteShader");
    spriteShader.bind();
    spriteShader.setMat4("projection", projection);

    Shader& particleShader = ResourceManager::getShader("particleShader");
    particleShader.bind();
    particleShader.setMat4("projection", projection);
}
