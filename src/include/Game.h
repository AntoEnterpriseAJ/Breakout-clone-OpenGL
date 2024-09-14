#pragma once
#include "SpriteRenderer.h"
#include "GLFW/glfw3.h"
#include "GameLevel.h"
#include "BallObject.h"
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

    void render();

    void init();
private:
    void processInput();
    bool checkCollision(const GameObject& obj1, const GameObject& obj2);
    bool checkCollision(const BallObject& ball, const GameObject& obj);
    void handleCollisions();
private:
    GLFWwindow* m_window;
	GameState m_state;
	unsigned int m_width, m_height;
	unsigned int m_currentLevel;

    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
    std::vector<GameLevel> m_levels;
    std::unique_ptr<GameObject> playerPaddle;
};