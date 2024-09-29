#include "PostProcessor.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

static constexpr unsigned int samples = 4;

PostProcessor::PostProcessor(const Shader& shader, unsigned int screenWidth, unsigned int screenHeight)
    : m_shader{shader}, m_screenWidth{screenWidth}, m_screenHeight{screenHeight}
{
    initMSFBO();
    initIFBO();
    initRenderData();
}

void PostProcessor::beginOffscreenRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::endOffscreenRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::initRenderData()
{
    float screenQuadVertices[] = {
        //positions     //texPos  
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
    };

    unsigned int screenQuadIndices[] = {
        0, 1, 2, 2, 3, 0
    };

    glGenVertexArrays(1, &m_screenQuadVAO);
    glBindVertexArray(m_screenQuadVAO);

    glGenBuffers(1, &m_screenQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &m_screenQuadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenQuadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenQuadIndices), screenQuadIndices, GL_STATIC_DRAW);
}

void PostProcessor::render() const
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_intermediaryFBO);
    glBlitFramebuffer(0, 0, m_screenWidth, m_screenHeight, 0, 0, m_screenWidth, m_screenHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindVertexArray(m_screenQuadVAO);
    m_shader.bind();
    m_shader.setInt("tex", 0);
    m_shader.setFloat("time", glfwGetTime());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_intermediaryColorBuffer);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void PostProcessor::startEffect(const std::string& effectName) const
{
    m_shader.setBool(effectName, true);
}

void PostProcessor::stopEffect(const std::string& effectName) const
{
    m_shader.setBool(effectName, false);
}

void PostProcessor::initMSFBO()
{
     // MSFBO:
    glGenFramebuffers(1, &m_MSFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_MSFBO);
    
    // MSFBO COLOR ATTACHMENT:
    glGenTextures(1, &m_MSColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_MSColorBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, m_screenWidth, m_screenHeight, GL_TRUE);
 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_MSColorBuffer, 0);

    // MSRBO ATTACHMENT:
    glGenRenderbuffers(1, &m_MSRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_MSRBO);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, m_screenWidth, m_screenHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_MSRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Succesffuly created the MSFBO" << std::endl;
    }
    else
    {
        std::cout << "ERROR::MSFBO: MSFBO creation failed\n" << std::endl;
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessor::initIFBO()
{
    // INTERMEDIARY FBO:
    glGenFramebuffers(1, &m_intermediaryFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_intermediaryFBO);
    
    // INTERMEDIARY COLOR ATTACHMENT:
    glGenTextures(1, &m_intermediaryColorBuffer);
    glBindTexture(GL_TEXTURE_2D, m_intermediaryColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_screenWidth, m_screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_intermediaryColorBuffer, 0);

    // INTERMEDIARY RBO:
    glGenRenderbuffers(1, &m_intermediaryRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_intermediaryRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_screenWidth, m_screenHeight);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_intermediaryRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Succesffuly created the INTERMEDIARY FBO" << std::endl;
    }
    else
    {
        std::cout << "ERROR::IFBO: INTERMEDIARY FBO creation failed\n" << std::endl;
        exit(-1);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
