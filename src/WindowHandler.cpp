#include <stdexcept>

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
    vkAcquireNextImageKHR (
        vulkan->device,
        vulkan->swapchain,
        UINT64_MAX,
        vulkan->imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &frameIndex
    );

    if (vkWaitForFences(vulkan->device, 1, &vulkan->fences[frameIndex], VK_FALSE, UINT64_MAX) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to wait for fences");
    }

    if (vkResetFences(vulkan->device, 1, &vulkan->fences[frameIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset fences!");
    }

    commandBuffer = vulkan->commandBuffers[frameIndex];
    image = vulkan->swapchainImages[frameIndex];
}

void WindowHandler::resetCommandBuffer()
{
    if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command buffer!");
    }
}

void WindowHandler::beginCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin command buffer!");
    }
}

void WindowHandler::endCommandBuffer()
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }
}

void WindowHandler::freeCommandBuffers()
{
    vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, 1, &commandBuffer);
}

void WindowHandler::beginRenderPass(VkClearColorValue clear_color, VkClearDepthStencilValue clear_depth_stencil)
{
    VkRenderPassBeginInfo renderPassInfo = {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass      = vulkan->renderPass,
        .framebuffer     = vulkan->swapchainFramebuffers[frameIndex],
        .renderArea {
            .offset      = {0, 0},
            .extent      = vulkan->swapchainSize,
        },
        .clearValueCount = 1,
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

    if (vkQueueSubmit(vulkan->graphicsQueue, 1, &submitInfo, vulkan->fences[frameIndex]) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command buffer!");
    }
}

void WindowHandler::queuePresent()
{
    VkPresentInfoKHR presentInfo {
        .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores    = &vulkan->renderingFinishedSemaphore,
        .swapchainCount     = 1,
        .pSwapchains        = &vulkan->swapchain,
        .pImageIndices      = &frameIndex,
        .pResults           = nullptr,
    };

    if (vkQueuePresentKHR(vulkan->presentQueue, &presentInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }

    if (vkQueueWaitIdle(vulkan->presentQueue) != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }
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
