#pragma once
#include "GameObject.h"

class BallObject : public GameObject
{
public:
    BallObject(const Texture2D& sprite, const glm::vec2& pos, const glm::vec2& velocity, float radius);

    void move(float deltaTime, unsigned int width);
    void leavePaddle();
    glm::vec2& getVelocityRef();

    bool onPaddle() const;
    glm::vec2 getVelocity() const;

private:
    glm::vec2 m_velocity;

    float m_radius;
    bool m_onPaddle;
};