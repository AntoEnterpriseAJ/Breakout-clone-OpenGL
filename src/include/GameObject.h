#pragma once
#include "glm/glm.hpp"
#include "Texture2D.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
    GameObject();
    GameObject(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, bool isBreakable);

    void draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer) const;
    glm::vec2 getPosition() const;
    glm::vec2 getSize() const;
    bool isBreakable() const;
    bool isDestroyed() const;

    glm::vec2& getPositionRef();
    void destroy();

protected:
    glm::vec2 m_position, m_size;
    glm::vec3 m_color;
    float m_rotationAngle;
    bool m_isBreakable, m_isDestroyed;

    const Texture2D& m_sprite;
};