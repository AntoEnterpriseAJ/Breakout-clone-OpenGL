#pragma once
#include "glm/glm.hpp"
#include "BallObject.h"
#include "GameObject.h"

// TODO: PROBABLY A SINGLETON

class CollisionManager
{
public:
    enum class Direction
    {
        UP      = 0,
        RIGHT   = 1,
        DOWN    = 2,
        LEFT    = 3,
    };

    struct CollisionStatus
    {
        bool        collision;
        Direction   direction;
        glm::vec2   offsetVec;     // get the object out of the object it collides with
    };

    CollisionStatus getCollisionStatus(const BallObject& ball, const GameObject& obj); // AABB Circle collision
    bool            getCollisionStatus(const GameObject& obj1, const GameObject& obj2); // AABB
    Direction       getVectorDirection(const glm::vec2& vec) const;
};