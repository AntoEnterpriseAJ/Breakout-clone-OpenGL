#include "GameLevel.h"
#include "ResourceManager.h"
#include <fstream>

void GameLevel::loadFromFile(const char* filePath, unsigned int levelHeight, unsigned int levelWidth)
{
    m_bricks.clear();

    std::ifstream fin(filePath);

    if (!fin.is_open())
    {
        std::cout << "ERROR::GAME_LEVEL: can't open level at path: " << filePath << std::endl;
        exit(-1);
    }

    std::vector<std::vector<unsigned int>> level;

    std::string line;
    unsigned int brickType;
    while (std::getline(fin, line))
    {
        std::stringstream sstream;
        sstream << line;
        std::vector<unsigned int> row;

        while (sstream >> brickType)
        {
            row.push_back(brickType);
        }

        level.push_back(row);
    }

    this->init(level, levelHeight, levelWidth);
}

void GameLevel::draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer)
{
    for (const auto& brick : m_bricks)
    {
        if (brick.isDestroyed())
            continue;

        brick.draw(spriteRenderer);
    }
}

void GameLevel::reset()
{
    for (auto& brick : m_bricks)
    {
        brick.reset();
    }
}

std::vector<GameObject>& GameLevel::getBricks()
{
    return m_bricks;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> level, unsigned int levelHeight, unsigned int levelWidth)
{
    unsigned int height = level.size();
    unsigned int width  = level[0].size();

    float brickWidth  = static_cast<float>(levelWidth) / width;
    float brickHeight = static_cast<float>(levelHeight) / height;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            unsigned int brickType = level[y][x];

            if (brickType == 0)
                continue;

            glm::vec2 position{x * brickWidth, y * brickHeight};
            glm::vec2 size{brickWidth, brickHeight};
            glm::vec3 color{0.0f};
            bool isBreakable = true;

            if (brickType == 1)
            {
                color = {0.8f, 0.8f, 0.7f};
                isBreakable = false;
            }
            else if (brickType == 2)
            {
                color = glm::vec3(0.2f, 0.6f, 1.0f);
            }
            else if (brickType == 3)
            {
                color = glm::vec3(0.0f, 0.7f, 0.0f);
            }
            else if (brickType == 4)
            {
                color = glm::vec3(0.8f, 0.8f, 0.4f);
            }
            else if (brickType == 5)
            {
                color = glm::vec3(1.0f, 0.5f, 0.0f);
            }

            Texture2D& texture = isBreakable ? ResourceManager::getTexture("block") : ResourceManager::getTexture("block_solid");

            m_bricks.emplace_back(texture, position, size, color, isBreakable);
        }
    }
}

