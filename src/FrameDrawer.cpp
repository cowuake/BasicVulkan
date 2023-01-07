#include <stdexcept>

#include "FrameDrawer.h"

FrameDrawer::FrameDrawer(SDL_Window *window, char *name)
{
    sdlWindow = window;
    windowName = name;

    clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
    clearDepthStencil = {1.0f, 0};

    vulkan = new VulkanHandler(sdlWindow, windowName);
    vulkan->init();
}

FrameDrawer::FrameDrawer(GLFWwindow *window, char *name)
{
    glfwWindow = window;
    windowName = name;

    vulkan = new VulkanHandler(glfwWindow, windowName);
    vulkan->init();
}

FrameDrawer::~FrameDrawer() {}

void FrameDrawer::acquireNextImage()
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

void FrameDrawer::resetCommandBuffer()
{
    if (vkResetCommandBuffer(commandBuffer, 0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to reset command buffer!");
    }
}

void FrameDrawer::beginCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
    };

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to begin command buffer!");
    }
}

void FrameDrawer::endCommandBuffer()
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        throw std::runtime_error("");
    }
}

void FrameDrawer::freeCommandBuffers()
{
    vkFreeCommandBuffers(vulkan->device, vulkan->commandPool, 1, &commandBuffer);
}

void FrameDrawer::beginRenderPass()
{
    std::vector<VkClearValue> clearValues(2);
    clearValues[0].color = clearColor;
    clearValues[1].depthStencil = clearDepthStencil;

    VkRenderPassBeginInfo renderPassInfo {
        .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass      = vulkan->renderPass,
        .framebuffer     = vulkan->swapchainFramebuffers[frameIndex],
        .renderArea {
            .offset      = {0, 0},
            .extent      = vulkan->swapchainSize,
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues    = clearValues.data(),
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void FrameDrawer::bindGraphicsPipelineToCommandBuffer()
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan->graphicsPipeline);
}

void FrameDrawer::endRenderPass()
{
    vkCmdEndRenderPass(commandBuffer);
}

void FrameDrawer::queueSubmit()
{
    VkSubmitInfo submitInfo {
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

void FrameDrawer::queuePresent()
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

void FrameDrawer::setViewport()
{
    VkViewport viewport {
        .x        = 0,
        .y        = 0,
        .width    = (float)vulkan->swapchainSize.width / 2,
        .height   = (float)vulkan->swapchainSize.height,
        .minDepth = (float)0.0f,
        .maxDepth = (float)1.0f,
    };

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void FrameDrawer::setScissor()
{
    VkRect2D scissor {
        .offset {
            .x      = 0,
            .y      = 0,
        },
        .extent {
            .width  = static_cast<uint32_t>(vulkan->swapchainSize.width / 2),
            .height = static_cast<uint32_t>(vulkan->swapchainSize.height),
        },
    };

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void FrameDrawer::draw()
{
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);
}

void FrameDrawer::setClearColor(int R, int G, int B, int A)
{
    clearColor = {(float)R/255, (float)G/255, (float)B/255, (float)A/255};
}

void FrameDrawer::setClearColor(int R, int G, int B)
{
    setClearColor(R, G, B, 255);
}

void FrameDrawer::nextFrame()
{
    acquireNextImage();

    resetCommandBuffer();
    beginCommandBuffer();

    beginRenderPass();
    bindGraphicsPipelineToCommandBuffer();
    setViewport();
    setScissor();
    draw();
    endRenderPass();

    endCommandBuffer();

    queueSubmit();
    queuePresent();
}