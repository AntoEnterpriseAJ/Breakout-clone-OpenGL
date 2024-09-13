#pragma once
#include "GameObject.h"

class BallObject : public GameObject
{
public:
    BallObject(const Texture2D& sprite, const glm::vec2& pos, const glm::vec2& velocity, float radius);

    void move(float deltaTime, unsigned int width);
    void leavePaddle();

    bool onPaddle() const;

private:
    glm::vec2 m_velocity;

    float m_radius;
    bool m_onPaddle;
};