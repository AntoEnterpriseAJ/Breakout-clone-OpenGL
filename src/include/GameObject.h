#pragma once
#include "glm/glm.hpp"
#include "Texture2D.h"
#include "SpriteRenderer.h"
#include <memory>

class GameObject
{
public:
    GameObject();
    GameObject(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size,
               glm::vec3 color = {1.0f, 1.0f, 1.0f}, bool isBreakable = true);

    GameObject& operator =(const GameObject& other);

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

    std::shared_ptr<Texture2D> m_sprite;
};