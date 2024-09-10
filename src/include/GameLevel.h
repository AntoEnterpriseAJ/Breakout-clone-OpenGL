#pragma once
#include "GameObject.h"
#include "SpriteRenderer.h"
#include <memory>

class GameLevel
{
public:
    GameLevel() = default;

    void loadFromFile(const char* filePath, unsigned int levelHeight, unsigned int levelWidth);
    void draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer);

private:
    void init(std::vector<std::vector<unsigned int>> level, unsigned int levelHeight, unsigned int levelWidth);
private:
    std::vector<GameObject> m_bricks;
};