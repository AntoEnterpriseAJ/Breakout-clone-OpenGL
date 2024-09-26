#pragma once
#include "SpriteRenderer.h"
#include "GLFW/glfw3.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "CollisionManager.h"
#include "PowerUp.h"
#include <array>
#include <memory>

class Game
{
public:
    enum GameState
    {
        GAME_ACTIVE,
        GAME_MENU,
        GAME_WIN,
    };

    Game(GLFWwindow* window,unsigned int width, unsigned int height);

    void update();
    void render();

private:
    void init();
    void processInput();
    void trySpawnPowerUp(const GameObject& brick);
    void updatePowerUps();
    void activatePowerUp(PowerUp& powerUp);
    void handleCollisions();
private:
    GLFWwindow*  m_window;
    GameState    m_state;
    unsigned int m_width, m_height;
    unsigned int m_currentLevel;

    CollisionManager                m_collisionManager;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::vector<GameLevel>          m_levels;
    std::array<PowerUp, 32>         m_powerUps;

    std::unique_ptr<GameObject>     playerPaddle;
};