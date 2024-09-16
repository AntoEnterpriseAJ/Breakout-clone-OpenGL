#include "ParticleGenerator.h"
#include "ResourceManager.h"
#include "glad/glad.h"

static constexpr unsigned int spawnsPerFrame = 1;
static unsigned int lastUsedParticle = 0;

ParticleGenerator::ParticleGenerator(const Particle& particle, int maxParticleCount)
    : m_particleLifeTime{particle.lifeTime}
{
    m_particles.resize(maxParticleCount, particle);

    float vertices[] = { 
    // pos          // tex
    0.0f, 0.0f,     0.0f, 0.0f, 
    1.0f, 0.0f,     1.0f, 0.0f,
    0.0f, 1.0f,     0.0f, 1.0f,
    1.0f, 1.0f,     1.0f, 1.0f,
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 1 
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    unsigned int VBO, EBO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
}

void ParticleGenerator::render(const Shader& particleShader)
{
    particleShader.use();
    particleShader.setInt("particleTexture", 0);
    particleShader.setVec2("scale", {30.0f, 30.0f});


    glActiveTexture(GL_TEXTURE0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (const auto& particle : m_particles)
    {
        particleShader.setVec2("offset", particle.position);
        glBindVertexArray(m_VAO);
        
        particle.sprite.bind();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// TODO: make it so that more spawns don't overwrite the particles currently in the buffer
void ParticleGenerator::update(const BallObject& obj, float deltaTime)
{
    for (int i = 0; i < spawnsPerFrame; ++ i)
    {
        int unusedParticleIndex = firstUnusedParticle();
        spawnParticle(m_particles[unusedParticleIndex], obj);
    }

    for (auto& particle : m_particles)
    {
        if (particle.isAlive())
        {
            particle.position += particle.velocity * deltaTime;
            particle.lifeTime -= particle.fadeRate * deltaTime;
        }
    }
}

int ParticleGenerator::firstUnusedParticle() const
{
    for (unsigned int index = lastUsedParticle; index < m_particles.size(); ++index)
    {
        if (!m_particles[index].isAlive())
        {
            lastUsedParticle = index;
            return index;
        }
    }

    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::spawnParticle(Particle& particle, const BallObject& obj)
{
    particle.velocity = -obj.getVelocity() * 0.1f;
    particle.position = obj.getPosition() + glm::vec2{rand() % 20 - 10, rand() % 20 - 10} + particle.velocity;
    particle.lifeTime = m_particleLifeTime;
}
