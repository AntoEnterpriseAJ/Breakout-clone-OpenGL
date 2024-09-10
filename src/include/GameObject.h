#pragma once
#include "glm/glm.hpp"
#include "Texture2D.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
    GameObject();
    GameObject(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, bool isSolid);

    void draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer) const;
    glm::vec2& getPositionRef();

private:
    glm::vec2 m_position, m_size, m_velocity;
    glm::vec3 m_color;
    float m_rotationAngle;
    bool m_isSolid, m_isDestroyed;

    const Texture2D& m_sprite;
};