#include "GameObject.h"

GameObject::GameObject()
    : m_sprite{}, m_position{0.0f}, m_size{0.0f}, m_color{1.0f}
    , m_rotationAngle{0.0f}, m_isBreakable{false}, m_isDestroyed{true}
{}

GameObject::GameObject(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, glm::vec3 color, bool isBreakable)
    : m_sprite{std::make_shared<Texture2D>(sprite)}, m_position{pos}, m_size{size}, m_color{color}
    , m_rotationAngle{0.0f}, m_isBreakable{isBreakable}, m_isDestroyed{false} 
{}

GameObject& GameObject::operator=(const GameObject & other)
{
    if (this != &other)
    {
        m_position      = other.m_position;
        m_size          = other.m_size;
        m_color         = other.m_color;
        m_rotationAngle = other.m_rotationAngle;
        m_isBreakable   = other.m_isBreakable;
        m_isDestroyed   = other.m_isDestroyed;
        m_sprite        = other.m_sprite;
    }

    return *this;
}

void GameObject::draw(const std::unique_ptr<SpriteRenderer>& spriteRenderer) const
{
    spriteRenderer->drawSprite(*m_sprite, m_position, m_size, m_rotationAngle, m_color);
}

glm::vec2 GameObject::getPosition() const
{
    return m_position;
}

glm::vec2 GameObject::getSize() const
{
    return m_size;
}

bool GameObject::isBreakable() const
{
    return m_isBreakable;
}

bool GameObject::isDestroyed() const
{
    return m_isDestroyed;
}

glm::vec2& GameObject::getPositionRef()
{
    return m_position;
}

glm::vec2& GameObject::getSizeRef()
{
    return m_size;
}

void GameObject::reset()
{
    m_isDestroyed = false;
}

void GameObject::destroy()
{
    m_isDestroyed = true;
}
