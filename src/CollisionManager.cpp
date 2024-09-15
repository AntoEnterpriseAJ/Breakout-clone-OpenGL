#include "CollisionManager.h"

CollisionManager::CollisionStatus CollisionManager::getCollisionStatus(const BallObject& ball, const GameObject& obj)
{
    float halfWidth  = obj.getSize().x / 2.0f;
    float halfHeight = obj.getSize().y / 2.0f;

    glm::vec2 rectangleCenter = obj.getPosition() + obj.getSize() / 2.0f;
    glm::vec2 ballCenter      = ball.getPosition() + ball.getSize() / 2.0f;
    float     ballRadius      = glm::length(ball.getSize() / 2.0f);

    glm::vec2 clampVector{halfWidth, halfHeight};

    glm::vec2 clampedFromRecToBall = glm::clamp(ballCenter - rectangleCenter, -clampVector, clampVector);
    glm::vec2 closestPoint = rectangleCenter + clampedFromRecToBall;

    float distance = glm::distance(closestPoint, ballCenter);

    if (distance < ballRadius)
    {
        glm::vec2 offset = ballCenter - closestPoint;
        offset = glm::normalize(offset);
        offset *= ballRadius - distance;

        return CollisionStatus{true, getVectorDirection(offset), offset};
    }
 
    return CollisionStatus{false, {}, {}};
}

CollisionManager::Direction CollisionManager::getVectorDirection(const glm::vec2& vec) const
{
    glm::vec2 directions[] = {
        glm::vec2{ 0.0f,  1.0f},
        glm::vec2{ 1.0f,  0.0f},
        glm::vec2{ 0.0f, -1.0f},
        glm::vec2{-1.0f,  0.0f},
    };

    int bestMatch = 0;
    float max     = -1.0f;
    for (int i = 0; i < 4; ++i)
    {
        float dot = glm::dot(directions[i], vec);
        if (dot > max)
        {
            max = dot;
            bestMatch = i;
        }
    }

    return static_cast<Direction>(bestMatch);
}
