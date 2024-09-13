#include "GameObject.h"

GameObject::GameObject()
    : m_sprite{}, m_position{0.0f}, m_size{0.0f},
      m_color{1.0f}, m_rotationAngle{0.0f}, m_isSolid{false}, m_isDestroyed{true} 
{}

GameObject::GameObject(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, bool isSolid)
    : m_sprite{sprite}, m_position{pos}, m_size{size},
      m_color{color}, m_rotationAngle{0.0f}, m_isSolid{isSolid}, m_isDestroyed{false} 
{}

void GameObject::draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer) const
{
    spriteRenderer->drawSprite(m_sprite, m_position, m_size, m_rotationAngle, m_color);
}

glm::vec2& GameObject::getPositionRef()
{
    return m_position;
}
