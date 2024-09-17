#include "ParticleGenerator.h"
#include "ResourceManager.h"
#include "glad/glad.h"

static constexpr float spawnRate            = 0.025;
static unsigned int lastUsedParticle        = 0;
static float timeSinceLastSpawn             = 0;

ParticleGenerator::ParticleGenerator(Particle& particle, int maxParticleCount)
    : m_particleLifeTime{particle.lifeTime}
{
    particle.lifeTime = 0;
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

    glActiveTexture(GL_TEXTURE0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBindVertexArray(m_VAO);

    for (const auto& particle : m_particles)
    {
        if (particle.isAlive())
        {
            particle.sprite.bind();
            particleShader.setFloat("scale", particle.size);
            particleShader.setVec2("offset", particle.position);

            float fadeFactor = particle.lifeTime / m_particleLifeTime;
            particleShader.setFloat("fadeFactor", fadeFactor);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::update(const BallObject& obj, float deltaTime)
{
    timeSinceLastSpawn += deltaTime;
    while (timeSinceLastSpawn >= spawnRate)
    {
        auto [canSpawn, index] = checkSpawn();
        if (canSpawn)
        {
            spawnParticle(m_particles[index], obj);
        }

        timeSinceLastSpawn -= spawnRate;
    }

    for (auto& particle : m_particles)
    {
        if (particle.isAlive())
        {
            particle.position += particle.velocity * deltaTime;
            particle.lifeTime -= particle.dieRate * deltaTime;
        }
    }
}

ParticleGenerator::BufferStatus ParticleGenerator::checkSpawn()
{
    unsigned int start = lastUsedParticle;
    do 
    {
        if (!m_particles[lastUsedParticle].isAlive())
        {
            return BufferStatus{true, lastUsedParticle};
        }
        lastUsedParticle = (lastUsedParticle + 1) % m_particles.size();
    } while (lastUsedParticle != start);

    return BufferStatus{false};
}

void ParticleGenerator::spawnParticle(Particle& particle, const BallObject& obj)
{
    particle.velocity = -obj.getVelocity() * 0.1f;
    particle.position = obj.getPosition() + glm::vec2{rand() % 20 - 10, rand() % 20 - 10} + particle.velocity;
    particle.lifeTime = m_particleLifeTime;
}
