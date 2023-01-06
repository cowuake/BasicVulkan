#ifndef VULKAN_FRAME_DRAWER_H_
#define VULKAN_FRAME_DRAWERp_H_

#include <SDL.h>
#include <vulkan/vulkan.h>

#include "VulkanHandler.h"

class FrameDrawer
{
private:
    SDL_Window *sdlWindow;
    GLFWwindow *glfwWindow;

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
    VulkanHandler *vulkan;

    FrameDrawer(SDL_Window *sdlWindow, char *sdlWindowName);
    FrameDrawer(GLFWwindow *glfwWindow, char *glfwWindowName);

    void setClearColor(int R, int G, int B, int A);
    void setClearColor(int R, int G, int B);
    void setClearDepthStencil();

    void drawNext();

    ~FrameDrawer();
};

#endif
