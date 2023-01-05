#include <thread>

#include <SDL.h>
#include <GLFW/glfw3.h>

#include "WindowHandler.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Application
{
public:
    WindowHandler* sdlHandler;
    SDL_Window* sdlWindow;
    SDL_Event event;
    char* sdlWindowName = std::string("SDL2 Vulkan Demo").data();

    WindowHandler* glfwHandler;
    GLFWwindow* glfwWindow;
    const char* glfwWindowName = std::string("GLFW Vulkan Demo").data();
    
    void init()
    {
        SDL_Init(SDL_INIT_EVERYTHING);
        sdlWindow = SDL_CreateWindow(
            sdlWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
            SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
        sdlHandler = new WindowHandler(sdlWindow, sdlWindowName);

        // glfwInit();
        // glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        // glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, glfwWindowName, nullptr, nullptr);
    }

    void mainLoop()
    {
        bool sdlRunning = true, glfwRunning = true;
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

            sdlHandler->acquireNextImage();
            sdlHandler->resetCommandBuffer();
            sdlHandler->beginCommandBuffer();

            VkClearColorValue clear_color = {r/255, g/255, b/255, 1.0f}; // RGBA
            VkClearDepthStencilValue clear_depth_stencil = {1.0f, 0};
            sdlHandler->beginRenderPass(clear_color, clear_depth_stencil);
            
            sdlHandler->endRenderPass();
            sdlHandler->endCommandBuffer();
            sdlHandler->queueSubmit();
            sdlHandler->queuePresent();
        }

        // while (glfwRunning)
        // {
        //     while (!glfwWindowShouldClose(glfwWindow))
        //     {
        //         glfwPollEvents();
        //     }

        //     glfwRunning = false;
        // }
    }

    void cleanup()
    {
        SDL_DestroyWindow(sdlWindow);
        sdlWindow = nullptr;
        delete sdlHandler;
        sdlHandler = nullptr;
        SDL_Quit();

        // glfwDestroyWindow(glfwWindow);
        // delete glfwHandler;
        // glfwHandler = nullptr;
        // glfwTerminate();
    }

public:
    void run()
    {
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
