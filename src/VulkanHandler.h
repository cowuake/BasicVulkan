#ifndef VULKAN_EXTERN_H_
#define VULKAN_EXTERN_H_

#include <iostream>
#include <vector>
#include <SDL.h>
#include <vulkan/vulkan.h>

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

class VulkanHandler
{
    private:
        SDL_Window *window;
        char *window_name;
        VkInstance instance;
        std::vector<VkExtensionProperties> instance_extension;
        VkDebugReportCallbackEXT debugCallback;
        VkSurfaceKHR surface;
        VkPhysicalDevice physical_devices;
        uint32_t graphicsQueueFamilyIndex;
        uint32_t presentQueueFamilyIndex;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        VkSurfaceFormatKHR surfaceFormat;
        uint32_t swapchainImageCount;
        std::vector<VkImageView> swapchainImageViews;
        VkFormat depthFormat;
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;
        PFN_vkCreateDebugReportCallbackEXT SDL2_vkCreateDebugReportCallbackEXT = nullptr;

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void createInstance();
        void createSemaphore(VkSemaphore *semaphore);
        void createDebug();
        void createSurface();
        void selectPhysicalDevice();
        void selectQueueFamily();
        void createDevice();
        void createSwapchain(bool resize);
        void createImageViews();
        void setupDepthStencil();
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
	    void createCommandBuffers();
	    void createSemaphores();
	    void createFences();

    public:
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFence> fences;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        std::vector<VkImage> swapchainImages;

        VkCommandPool commandPool;
        VkDevice device;
        VkExtent2D swapchainSize;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkRenderPass render_pass;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderingFinishedSemaphore;
        VkSwapchainKHR swapchain;

        VulkanHandler(SDL_Window *sdl_window, char *sdl_window_name);

        void init();

        ~VulkanHandler();
};

#endif
