#include <thread>

#include <SDL.h>
#include <GLFW/glfw3.h>

#include "WindowHandler.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Application
{
public:
    WindowHandler* handler;

    SDL_Window* sdlWindow;
    SDL_Event event;
    char* sdlWindowName = std::string("SDL2 Vulkan Demo").data();

    GLFWwindow* glfwWindow;
    const char* glfwWindowName = std::string("GLFW Vulkan Demo").data();
    bool sdlRunning, glfwRunning;
    
    void init()
    {
        sdlWindow = SDL_CreateWindow(
            sdlWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

        glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, glfwWindowName, NULL, NULL);
            
        handler = new WindowHandler(sdlWindow, sdlWindowName);
        sdlRunning = true;
    }

    void mainLoop()
    {
        float r = 112, g = 66, b = 20;

        while (sdlRunning)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    sdlRunning = false;
                }
            }

            handler->acquireNextImage();
            handler->resetCommandBuffer();
            handler->beginCommandBuffer();

            VkClearColorValue clear_color = {r/255, g/255, b/255, 1.0f}; // RGBA
            VkClearDepthStencilValue clear_depth_stencil = {1.0f, 0};
            handler->beginRenderPass(clear_color, clear_depth_stencil);
            
            handler->endRenderPass();
            handler->endCommandBuffer();
            handler->queueSubmit();
            handler->queuePresent();
        }
    }

    void cleanup()
    {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;

        delete handler;
        handler = nullptr;

        SDL_Quit();
    }

public:
    void run()
    {
        SDL_Init(SDL_INIT_EVERYTHING);
        init();
        mainLoop();
        cleanup();
    }
};

int main(int argc, char *argv[])
{
    Application app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
