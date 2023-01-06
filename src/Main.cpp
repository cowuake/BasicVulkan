#include <iostream>
#include <memory>
#include <thread>

#include <SDL.h>
#include <GLFW/glfw3.h>

#include "FrameDrawer.h"
#include "VulkanHandler.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

class Application
{
public:
    std::unique_ptr<FrameDrawer> sdlHandler, glfwHandler;
    SDL_Window *sdlWindow;
    GLFWwindow *glfwWindow;
    SDL_Event event;
    ApplicationType appType;

    Application(enum ApplicationType type)
    {
        appType = type;
    }

    void init()
    {
        if (appType == ApplicationType::SDL)
        {
            std::string sdlWindowNameStr = "SDL2 Vulkan Demo";
            char *sdlWindowName = sdlWindowNameStr.data();

            SDL_Init(SDL_INIT_EVERYTHING);

            sdlWindow = SDL_CreateWindow(
                sdlWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

            sdlHandler = std::unique_ptr<FrameDrawer>(new FrameDrawer(sdlWindow, sdlWindowName));
        }
        else if (appType == ApplicationType::GLFW)
        {
            std::string glfwWindowNameStr = "GLFW Vulkan Demo";
            const char *glfwWindowName = glfwWindowNameStr.data();

            glfwInit();
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

            glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, glfwWindowName, nullptr, nullptr);
        }
    }

    void mainLoop()
    {
        bool sdlRunning = true, glfwRunning = true;
        int r = 112, g = 66, b = 20;

        if (appType == SDL)
        {
            sdlHandler->setClearColor(r, g, b);

            while (sdlRunning)
            {
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                    {
                        sdlRunning = false;
                    }
                }

                sdlHandler->drawNext();
            }
        }
        else if (appType == GLFW)
        {
            while (glfwRunning)
            {
                while (!glfwWindowShouldClose(glfwWindow))
                {
                    glfwPollEvents();
                }

                glfwRunning = false;
            }
        }
    }

    void cleanup()
    {
        if (appType == SDL)
        {
            SDL_DestroyWindow(sdlWindow);
            sdlWindow = nullptr;
            sdlHandler.reset();
            SDL_Quit();
        }
        else if (appType == GLFW)
        {
            glfwDestroyWindow(glfwWindow);
            glfwHandler.reset();
            glfwTerminate();
        }
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
    Application sdlApp(ApplicationType::SDL);
    Application glfwApp(ApplicationType::GLFW);

    try
    {
        std::thread sdl {&Application::run, &sdlApp};
        std::thread glfw {&Application::run, &glfwApp};

        sdl.join(); glfw.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
