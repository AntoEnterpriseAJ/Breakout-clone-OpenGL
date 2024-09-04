#pragma once

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
	~Game();

private:
	GameState m_state;
	unsigned int m_width, m_height;
};