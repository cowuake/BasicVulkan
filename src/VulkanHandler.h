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

        VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void CreateInstance();
        void CreateSemaphore(VkSemaphore *semaphore);
        void CreateDebug();
        void CreateSurface();
        void SelectPhysicalDevice();
        void SelectQueueFamily();
        void CreateDevice();
        bool CreateSwapchain(bool resize);
        void CreateImageViews();
        void SetupDepthStencil();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateCommandPool();
	    void CreateCommandBuffers();        
	    void CreateSemaphores();
	    void CreateFences();

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

        void Init();

        ~VulkanHandler();
};

#endif
