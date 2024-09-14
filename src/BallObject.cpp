#include "BallObject.h"

BallObject::BallObject(const Texture2D& sprite, const glm::vec2& pos, const glm::vec2& velocity, float radius)
    : GameObject{sprite, pos, glm::vec2{2 * radius, 2 * radius}, glm::vec3{1.0f, 1.0f, 1.0f}, true}
    , m_radius{radius}, m_onPaddle{true}, m_velocity{velocity}
{}

void BallObject::move(float deltaTime, unsigned int width)
{
    if (!m_onPaddle)
    {
        if (m_position.y <= 0.0f)
        {
            m_velocity.y = -m_velocity.y;
        }
        else if (m_position.x <= 0.0f || m_position.x + 2 * m_radius >= width)
        {
            m_velocity.x = -m_velocity.x;
        }

        m_position += m_velocity * deltaTime;
    }
}

void BallObject::leavePaddle()
{
    m_onPaddle = false;
}

bool BallObject::onPaddle() const
{
    return m_onPaddle;
}


