#pragma once
#include "Texture2D.h"
#include "BallObject.h"
#include "glm/glm.hpp"
#include <vector>

class ParticleGenerator
{
public:
    struct Particle
    {
        const Texture2D& sprite;

        float       size;
        float       lifeTime;
        float       fadeRate;

        glm::vec2   velocity = {0.0f, 0.0f};
        glm::vec2   position = {0.0f, 0.0f};

        bool isAlive() const {return lifeTime > 0;};
    };

    ParticleGenerator(const Particle& particle, int maxParticleCount);

    void render(const Shader& particleShader);
    void update(const BallObject& obj, float deltaTime);

private:
    int firstUnusedParticle() const;
    void spawnParticle(Particle& particle, const BallObject& obj);
private:
    std::vector<Particle> m_particles;

    float m_particleLifeTime;
    unsigned int m_VAO;
};