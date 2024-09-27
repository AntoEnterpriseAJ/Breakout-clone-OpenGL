#include "PowerUp.h"
#include "GameConstants.h"

PowerUp::PowerUp()
    : GameObject{}, m_type{}, m_duration{0.0f}, m_active{false}
{}

PowerUp::PowerUp(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, PowerUp::Type type, float duration)
    : GameObject{sprite, pos, size}, m_type{type}, m_duration{duration}, m_active{false}
{}

PowerUp& PowerUp::operator=(const PowerUp& other)
{
    if (this != &other)
    {
        this->GameObject::operator=(other);
        m_type     = other.m_type;
        m_duration = other.m_duration;
    }

    return *this;
}

PowerUp::Type PowerUp::getType() const
{
    return m_type;
}

bool PowerUp::isActive() const
{
    return m_active;
}

float PowerUp::getDuration() const
{
    return m_duration;
}

void PowerUp::setDuration(float duration)
{
    m_duration = duration;
}

void PowerUp::updateMove(float deltaTime, unsigned int screenHeight)
{
    glm::vec2 vel = GameConstants::initPowerUpVel;

    if (m_position.y > screenHeight)
    {
        m_isDestroyed = true;
        return;
    }

    m_position.y += vel.y * deltaTime;
}

void PowerUp::updatEffectDuration(float deltaTime)
{
    if (m_active)
    {
        m_duration -= deltaTime;

        if (m_duration <= 0.0f)
        {
            m_active = false;
        }
    }
}

void PowerUp::activate()
{
    m_active = true;
}
