#include "WindowHandler.h"

WindowHandler::WindowHandler(SDL_Window *sdlWindow, char *sdlWindowName)
{
    window = sdlWindow;
    windowName = sdlWindowName;
    vulkan = new VulkanHandler(window, windowName);
    vulkan->init();
}

WindowHandler::~WindowHandler() {}

void WindowHandler::acquireNextImage()
{
    vkAcquireNextImageKHR(
        vulkan->device,
        vulkan->swapchain,
        UINT64_MAX,
        vulkan->imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &frameIndex);

    vkWaitForFences(vulkan->device, 1, &vulkan->fences[frameIndex], VK_FALSE, UINT64_MAX);
    vkResetFences(vulkan->device, 1, &vulkan->fences[frameIndex]);

    commandBuffer = vulkan->commandBuffers[frameIndex];
    image = vulkan->swapchainImages[frameIndex];
}

void WindowHandler::resetCommandBuffer()
{
    vkResetCommandBuffer(commandBuffer, 0);
}

void WindowHandler::beginCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void WindowHandler::endCommandBuffer()
{
    vkEndCommandBuffer(commandBuffer);
}

void WindowHandler::freeCommandBuffers()
{
    vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, 1, &commandBuffer);
}

void WindowHandler::beginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil)
{
	VkRenderPassBeginInfo renderPassInfo = {
        .sType        = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass   = vulkan->render_pass,
        .framebuffer  = vulkan->swapchainFramebuffers[frameIndex],
        .renderArea {
            .offset   = {0, 0},
            .extent   = vulkan->swapchainSize,
        },
        .clearValueCount   = 1,
    };
	
	std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = clear_color;
    clearValues[1].depthStencil = clear_depth_stencil;

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void WindowHandler::endRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void WindowHandler::queueSubmit()
{
    VkSubmitInfo submitInfo = {
        .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount   = 1,
        .pWaitSemaphores      = &vulkan->imageAvailableSemaphore,
        .pWaitDstStageMask    = &waitDestStageMask,
        .commandBufferCount   = 1,
        .pCommandBuffers      = &commandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores    = &vulkan->renderingFinishedSemaphore,
    };

    vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, vulkan->fences[frameIndex]);
}

void WindowHandler::queuePresent()
{
    VkPresentInfoKHR presentInfo = {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &vulkan->renderingFinishedSemaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &vulkan->swapchain,
        .pImageIndices      = &frameIndex,
    };
    
    vkQueuePresentKHR(vulkan->presentQueue, &presentInfo);
    vkQueueWaitIdle(vulkan->presentQueue);
}

void WindowHandler::setViewport(int width,int height)
{
    VkViewport viewport;
    viewport.width    = (float)width / 2;
    viewport.height   = (float)height;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    viewport.x        = 0;
    viewport.y        = 0;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void WindowHandler::setScissor(int width,int height)
{
    VkRect2D scissor;
    scissor.extent.width  = width / 2;
    scissor.extent.height = height;
    scissor.offset.x      = 0;
    scissor.offset.y      = 0;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}
