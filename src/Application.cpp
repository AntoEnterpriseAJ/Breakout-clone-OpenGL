#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "Game.h"

GLFWwindow* createWindow(unsigned int screenWidth, unsigned int screenHeight);
void initOpenGL();

static const unsigned int SCREEN_WIDTH  = 800;
static const unsigned int SCREEN_HEIGHT = 600;

int main(void)
{
	GLFWwindow* window = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
    initOpenGL();

    Game game{window, SCREEN_WIDTH, SCREEN_HEIGHT};

    while (!glfwWindowShouldClose(window))
    {
        game.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}