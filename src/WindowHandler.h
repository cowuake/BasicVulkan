#ifndef VULKAN_FUNCTION_H_
#define VULKAN_FUNCTION_H_

#include <SDL.h>
#include <vulkan/vulkan.h>
#include "VulkanHandler.h"

class WindowHandler
{
private:
    VulkanHandler *vulkan;
    SDL_Window *window;
    char *windowName;
    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    VkImage image;

public:
    WindowHandler(SDL_Window *sdlWindow, char *sdlWindowName);
    
    void AcquireNextImage();
    void ResetCommandBuffer();
    void BeginCommandBuffer();
    void EndCommandBuffer();
    void FreeCommandBuffers();
    void BeginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil);
    void EndRenderPass();
    void QueueSubmit();
    void QueuePresent();
    void SetViewport(int width, int height);
    void SetScissor(int width, int height);

    ~WindowHandler();
};

#endif
