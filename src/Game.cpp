#include "Game.h"
#include "ResourceManager.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "GameConstants.h"
#include "irrKlang/irrKlang.h"
#include "TextRenderer.h"
#include <thread>

static float deltaTime = 0.0f;
static float lastTime  = 0.0f;

static float shakeTime;

static std::unique_ptr<BallObject>             ball;
static std::unique_ptr<irrklang::ISoundEngine> soundEngine;
static std::unique_ptr<PostProcessor>          postProcessor; 
static std::unique_ptr<ParticleGenerator>      particleGenerator;
static std::unique_ptr<TextRenderer> 	       textRenderer;

Game::Game(GLFWwindow* window,unsigned int width, unsigned int height)
    : m_window{ window }, m_width{ width }, m_height{ height }, m_currentLevel{ 0 }, m_state{ GAME_MENU }, m_lives{ 3 }
{
    init();
}

void Game::render()
{

    if (m_state == GAME_ACTIVE)
    {
        update();
        postProcessor->beginOffscreenRendering();

        m_spriteRenderer->drawSprite(ResourceManager::getTexture("background"), {0.0f, 0.0f}, {m_width, m_height}, 0.0f);
        m_levels[m_currentLevel].draw(m_spriteRenderer);
        playerPaddle->draw(m_spriteRenderer);

        renderPowerUpSpawns();

        particleGenerator->render(ResourceManager::getShader("particleShader"));
        ball->draw(m_spriteRenderer);
        textRenderer->RenderText(ResourceManager::getShader("text"), "Lives: " + std::to_string(m_lives), 5.0f, 570.0f, 0.8);

        postProcessor->endOffscreenRendering();
        postProcessor->render();
    }

    if (m_state == GAME_MENU)
    {
        processInput();
        m_spriteRenderer->drawSprite(ResourceManager::getTexture("background"), { 0.0f, 0.0f }, { m_width, m_height }, 0.0f);
        m_levels[m_currentLevel].draw(m_spriteRenderer);
        playerPaddle->draw(m_spriteRenderer);
        ball->draw(m_spriteRenderer);

        textRenderer->RenderText(ResourceManager::getShader("text"), "Press ENTER to start", 170.0f, m_height / 2.0f, 0.8);
        textRenderer->RenderText(ResourceManager::getShader("text"), "Press W or S to select a level", 190.0f, m_height / 2.0f - 30.0f, 0.5f);
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

    if (ball->getPosition().y >= m_height)
    {
        --m_lives;
        if (m_lives <= 0)
        {
            m_levels[m_currentLevel].reset();
            m_state = GAME_MENU;
        }

        ball->stickToPaddle();
        ball->getPositionRef()         = GameConstants::initBallPos;
        ball->getVelocityRef()         = GameConstants::ballInitVel;
        playerPaddle->getPositionRef() = GameConstants::initPadPos;
        playerPaddle->getSizeRef()     = GameConstants::initPadSize;
    }

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

    if (m_state == GAME_ACTIVE)
    {
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
    else if (m_state == GAME_MENU)
    {
        if (glfwGetKey(m_window, GLFW_KEY_ENTER) == GLFW_PRESS)
        {
            m_state = GAME_ACTIVE;
            m_lives = 3;
        }

        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
		{
			m_currentLevel = (m_currentLevel - 1) % m_levels.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
        else if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_currentLevel = (m_currentLevel + 1) % m_levels.size();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    }
}

void Game::trySpawnPowerUp(const GameObject& brick)
{
    if (rand() % 100 <= 30)
    {
        glm::vec2 size = {50.0f, 20.0f};
        glm::vec2 spawnPoint = brick.getPosition() + glm::vec2{brick.getSize().x, brick.getSize().y} / 2.0f - size / 2.0f;

        for (int i = 0; i < m_powerUpSpawns.size(); ++i)
        {
            if (!m_powerUpSpawns[i].isDestroyed())
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
            else if (effect == PowerUp::Type::increase)
            {
                duration = GameConstants::effectIncreaseTime;
                texture = "powerup_increase";
            }

            m_powerUpSpawns[i] = PowerUp{ResourceManager::getTexture(texture), spawnPoint, size, effect, duration};
            return;
        }
    }
}

void Game::updatePowerUps()
{
    ResourceManager::getShader("postFX").bind();

    for (auto& powerUp : m_powerUpSpawns)
    {
        if (!powerUp.isDestroyed())
            powerUp.updateMove(deltaTime, m_height);
    }

    for (auto& [type, powerUp] : m_lastEffect)
    {
        if (powerUp.isActive())
        {
            powerUp.updatEffectDuration(deltaTime);

            if(!powerUp.isActive())
            {
                if (powerUp.getType() == PowerUp::Type::speedUp)
                {
                    ball->getVelocityRef() /= GameConstants::speedUpMultiplier;
                }
                else if (powerUp.getType() == PowerUp::Type::increase)
                {
                    float oldWidth = playerPaddle->getSize().x;

                    playerPaddle->getPositionRef().x += (oldWidth - GameConstants::initPadSize.x) / 2.0f;
                    playerPaddle->getSizeRef().x /= GameConstants::effectIncreaseMultiplier;
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
    ResourceManager::getShader("postFX").bind();
    powerUp.activate();
    PowerUp::Type effect = powerUp.getType();

    if (m_lastEffect[effect].getDuration() > 0)
    {
        m_lastEffect[effect].setDuration(powerUp.getDuration());
    }
    else
    {
        m_lastEffect[effect] = powerUp;
        m_lastEffect[effect].activate();

        if (powerUp.getType() == PowerUp::Type::speedUp)
        {
            ball->getVelocityRef() *= GameConstants::speedUpMultiplier;
        }
        else if (powerUp.getType() == PowerUp::Type::increase)
        {
            float oldWidth = playerPaddle->getSize().x;
            float newWidth = oldWidth * GameConstants::effectIncreaseMultiplier;

            playerPaddle->getPositionRef().x -= (newWidth - oldWidth) / 2.0f;
            playerPaddle->getSizeRef().x = newWidth;
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
                soundEngine->play2D("res/audio/solid.wav", false);
                ResourceManager::getShader("postFX").bind();
                postProcessor->startEffect("effectShake");
                shakeTime = GameConstants::shakeTime;
            }

            if (brick.isBreakable())
            {
                soundEngine->play2D("res/audio/bleep.mp3", false);
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
            soundEngine->play2D("res/audio/bleep.wav", false);
            glm::vec2 paddleCenter = playerPaddle->getPosition() + playerPaddle->getSize() / 2.0f;
            glm::vec2 ballCenter   = ball->getPosition() + ball->getSize() / 2.0f;

            glm::vec2 bounceDirection = glm::normalize(ballCenter - paddleCenter);

            ball->getVelocityRef() = bounceDirection * glm::length(ball->getVelocity());
        }
    }

    for (auto& powerUp : m_powerUpSpawns)
    {
        if (powerUp.isDestroyed())
            continue;

        bool collision = m_collisionManager.getCollisionStatus(*playerPaddle, powerUp);
        if (collision)
        {
            soundEngine->play2D("res/audio/powerup.wav", false);
            activatePowerUp(powerUp);
            powerUp.destroy();
        }
    }
}

void Game::renderPowerUpSpawns()
{
    for (auto& powerUp : m_powerUpSpawns)
    {
        if (!powerUp.isDestroyed())
        {
            powerUp.draw(m_spriteRenderer);
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
    ResourceManager::loadTexture("res/textures/powerup_increase.png", "powerup_increase");

    ResourceManager::loadShader("res/shaders/sprite.vert", "res/shaders/sprite.frag", "spriteShader");
    ResourceManager::loadShader("res/shaders/particle.vert", "res/shaders/particle.frag", "particleShader");
    ResourceManager::loadShader("res/shaders/postFX.vert", "res/shaders/postFX.frag", "postFX");
    ResourceManager::loadShader("res/shaders/text.vert", "res/shaders/text.frag", "text");

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
    soundEngine         = std::unique_ptr<irrklang::ISoundEngine>(irrklang::createIrrKlangDevice());
    textRenderer        = std::make_unique<TextRenderer>( "res/fonts/Astron.otf", 40);


    soundEngine->setSoundVolume(0.15f);
    soundEngine->play2D("res/audio/theme.mp3", true);

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
