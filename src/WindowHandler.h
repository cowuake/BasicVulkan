#ifndef VULKAN_FUNCTION_H_
#define VULKAN_FUNCTION_H_

#include <SDL.h>
#include <vulkan/vulkan.h>

#include "VulkanHandler.h"

class FrameDrawer
{
private:
    VulkanHandler *vulkan;
    SDL_Window *window;
    char *windowName;
    uint32_t frameIndex;
    VkCommandBuffer commandBuffer;
    VkImage image;
    VkPipelineStageFlags waitDestStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkClearColorValue clearColor;
    VkClearDepthStencilValue clearDepthStencil;

    void acquireNextImage();
    void resetCommandBuffer();
    void beginCommandBuffer();
    void endCommandBuffer();
    void freeCommandBuffers();
    void beginRenderPass();
    void endRenderPass();
    void queueSubmit();
    void queuePresent();
    void setViewport(int width, int height);
    void setScissor(int width, int height);

public:
    FrameDrawer(SDL_Window *sdlWindow, char *sdlWindowName);

    void setClearColor(int R, int G, int B, int A);
    void setClearDepthStencil();

    void drawNext();

    ~FrameDrawer();
};

#endif
