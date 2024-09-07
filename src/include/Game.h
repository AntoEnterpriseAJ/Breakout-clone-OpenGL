#pragma once
#include "SpriteRenderer.h"
#include <memory>

enum GameState
{
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
};

class Game
{
public:
	Game(unsigned int width, unsigned int height);

    void render();

private:
    void init();
private:
	GameState m_state;
	unsigned int m_width, m_height;
    std::unique_ptr<SpriteRenderer> m_spriteRenderer;
};