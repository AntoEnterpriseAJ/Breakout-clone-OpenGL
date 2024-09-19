#include <iostream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "glm/glm.hpp"
#include "Game.h"
#include "ResourceManager.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, 
                            GLsizei length, const char *message, const void *userParam);

static const unsigned int SCREEN_WIDTH = 800;
static const unsigned int SCREEN_HEIGHT = 600;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "BreakoutClone", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetWindowTitle(window, "BreakoutCLone");
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "WARNING: Failed to initialize GLAD" << std::endl;
        return -1;
    }
    else
    {
        int minor, major;
        glfwGetVersion(&minor, &major, nullptr);
        std::cout << "Succesffuly initialized GLAD\nOpenGL " << major << "." << minor << std::endl;
    }

    int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        std::cout << "Succesfully initialized debug output" << std::endl;
    }
    else 
        std::cout << "WARNING: Failed to initialize debug output" << std::endl;

    //glfwSwapInterval(0); 
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // FBO:
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // COLOR ATTACHMENT:
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    // DEPTH AND STENCIL RBO ATTACHMENT:
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT , GL_RENDERBUFFER, RBO);

    if (glad_glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Succesffuly created the framebuffer" << std::endl;
    }
    else
    {
        std::cout << "WARNING: Framebuffer creation failed\n" << std::endl;
        exit(-1);
    }

    // Generate grid vertices
    std::vector<float> gridVertices;
    int gridSpacing = 5;  // 1-pixel spacing grid
    for (int x = 0; x <= SCREEN_WIDTH; x += gridSpacing)
    {
        float xNormalized = (2.0f * x / SCREEN_WIDTH) - 1.0f;

        gridVertices.push_back(xNormalized);
        gridVertices.push_back(1.0f);
        gridVertices.push_back(xNormalized);
        gridVertices.push_back(-1.0f);
    }
    for (int y = 0; y <= SCREEN_HEIGHT; y += gridSpacing)
    {
        float yNormalized = (2.0f * y / SCREEN_HEIGHT) - 1.0f;

        gridVertices.push_back(-1.0f);
        gridVertices.push_back(yNormalized);
        gridVertices.push_back(1.0f);
        gridVertices.push_back(yNormalized);
    }

    int numGridVertices = gridVertices.size() / 2;  // Number of vertices

    // Set up VAO and VBO
    ResourceManager::loadShader("res/shaders/test.vert", "res/shaders/test.frag", "testShader");
    ResourceManager::getShader("testShader").use();

    unsigned int gridVAO, gridVBO;
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);

    glBindVertexArray(gridVAO);

    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), &gridVertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // SCREEN QUAD
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

    unsigned int screenQuadVAO, screenQuadVBO, screenQuadEBO;
    glGenVertexArrays(1, &screenQuadVAO);
    glBindVertexArray(screenQuadVAO);

    glGenBuffers(1, &screenQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)2);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &screenQuadEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screenQuadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(screenQuadIndices), screenQuadIndices, GL_STATIC_DRAW);

    // postFX shader
    ResourceManager::loadShader("res/shaders/postFX.vert", "res/shaders/postFX.frag", "postFX");
    Shader& shaderPostFX = ResourceManager::getShader("postFX");

    Game game{window, SCREEN_WIDTH, SCREEN_HEIGHT};

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // WIREFRAME MODE

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        game.render();
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shaderPostFX.use();
        shaderPostFX.setInt("tex", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, screenQuadIndices);

        //ResourceManager::getShader("testShader").use();

        //glBindVertexArray(gridVAO);
        //glDrawArrays(GL_LINES, 0, numGridVertices);
        //glBindVertexArray(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteFramebuffers(1, &FBO);

    glfwTerminate();
    return 0;
}