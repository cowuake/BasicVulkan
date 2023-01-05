#ifndef VULKAN_EXTERN_H_
#define VULKAN_EXTERN_H_

#include <iostream>
#include <vector>
#include <SDL.h>
#include <vulkan/vulkan.h>

using namespace std;

#define CLAMP(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))

class VulkanHandler
{
    private:
        SDL_Window *window;
        char * window_name;
        VkInstance instance;
        vector<VkExtensionProperties> instance_extension;
        VkDebugReportCallbackEXT debugCallback;
        VkSurfaceKHR surface;
        VkPhysicalDevice physical_devices;
        uint32_t graphics_QueueFamilyIndex;
        uint32_t present_QueueFamilyIndex;
        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        VkSurfaceFormatKHR surfaceFormat;
        uint32_t swapchainImageCount;
        vector<VkImageView> swapchainImageViews;
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
        bool createSwapchain(bool resize);
        void createImageViews();
        void setupDepthStencil();
        void createRenderPass();
        void createFramebuffers();
        void createCommandPool();
	    void createCommandBuffers();        
	    void createSemaphores();
	    void createFences();

    public:
        vector<VkCommandBuffer> commandBuffers;
        vector<VkFence> fences;
        vector<VkFramebuffer> swapchainFramebuffers;
        vector<VkImage> swapchainImages;

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
