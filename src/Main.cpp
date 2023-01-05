#include <memory>
#include <thread>

#include <SDL.h>
#include <GLFW/glfw3.h>

#include "WindowHandler.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

enum applicationType { SDL, GLFW };

class Application
{
public:
    std::unique_ptr<WindowHandler> sdlHandler, glfwHandler;
    SDL_Window* sdlWindow;
    SDL_Event event;
    GLFWwindow* glfwWindow;
    applicationType appType;

    Application(enum applicationType type)
    {
        appType = type;
    }
    
    void init()
    {
        if (appType == SDL)
        {
            std::string sdlWindowNameStr = "SDL2 Vulkan Demo";
            char *sdlWindowName = sdlWindowNameStr.data();

            SDL_Init(SDL_INIT_EVERYTHING);
            sdlWindow = SDL_CreateWindow(
                sdlWindowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
            sdlHandler = std::unique_ptr<WindowHandler>(new WindowHandler(sdlWindow, sdlWindowName));
        }
        else if (appType == GLFW)
        {
            std::string glfwWindowNameStr = std::string("GLFW Vulkan Demo");
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
        float r = 112, g = 66, b = 20;

        if (appType == SDL)
        {
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

                VkClearColorValue clearColor = {r / 255, g / 255, b / 255, 1.0f}; // RGBA
                VkClearDepthStencilValue clearDepthStencil = {1.0f, 0};
                sdlHandler->beginRenderPass(clearColor, clearDepthStencil);

                sdlHandler->endRenderPass();
                sdlHandler->endCommandBuffer();
                sdlHandler->queueSubmit();
                sdlHandler->queuePresent();
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
    Application sdlApp(applicationType::SDL);
    Application glfwApp(applicationType::GLFW);

    try
    {
        std::thread t1 {&Application::run, &sdlApp};
        std::thread t2 {&Application::run, &glfwApp};

        t1.join(); t2.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
