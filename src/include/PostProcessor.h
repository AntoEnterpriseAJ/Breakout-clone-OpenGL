#pragma once
#include "Shader.h"

class PostProcessor
{
public:
    enum class Effect
    {
        normal   ,
        inversion,
        count    ,
    };

    PostProcessor(const Shader& shader, unsigned int screenWidth, unsigned int screenHeight);

    void beginOffscreenRendering();
    void endOffscreenRendering();
    void render() const;
private:
    void initMSFBO();
    void initIFBO();
    void initRenderData();
private:
    unsigned int m_MSFBO        , m_intermediaryFBO;
    unsigned int m_MSColorBuffer, m_intermediaryColorBuffer; 
    unsigned int m_MSRBO        , m_intermediaryRBO;

    unsigned int m_screenWidth  , m_screenHeight;
    const Shader& m_shader;

    unsigned int m_screenQuadVAO, m_screenQuadEBO, m_screenQuadVBO;
};
