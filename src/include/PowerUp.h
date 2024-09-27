#pragma once
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "PostProcessor.h"
#include <memory>

class PowerUp : public GameObject
{
public:
    enum class Type
    {
        speedUp,
        confuse,
        chaos,
        count,
    };

    PowerUp();
    PowerUp(const Texture2D& sprite, glm::vec2 pos, glm::vec2 size, PowerUp::Type type, float duration);
    PowerUp& operator=(const PowerUp& other);
    
    Type  getType() const;
    bool  isActive() const;
    float getDuration() const;

    void setDuration(float duration);
    void updateMove(float deltaTime, unsigned int screenHeight);
    void updatEffectDuration(float deltaTime);
    void activate();

private:
    Type    m_type;
    bool    m_active;
    float   m_duration;
};
