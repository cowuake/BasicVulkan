#ifndef VULKAN_HANDLER_H_
#define VULKAN_HANDLER_H_

#include <vector>

#include <SDL.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

enum ApplicationType { SDL, GLFW };

class VulkanHandler
{
    private:
        SDL_Window *sdlWindow;
        GLFWwindow *glfwWindow;
        char *windowName;
        enum ApplicationType applicationType;

        VkInstance instance;
        std::vector<VkExtensionProperties> instance_extension;
        VkDebugReportCallbackEXT debugCallback;
        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice;
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
        PFN_vkCreateDebugReportCallbackEXT SDL2_vkCreateDebugReportCallbackEXT;
        VkPipelineLayout pipelineLayout;
        VkPipeline graphicsPipeline;

        VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat *depthFormat);

        void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
        void createInstance();
        void createDebug();
        void createSurface();
        void selectPhysicalDevice();
        void selectQueueFamily();
        void createDevice();
        void createSwapchain(bool resize);
        void createImageViews();
        void setupDepthStencil();
        void createRenderPass();
        VkShaderModule createShaderModule(const std::vector<char> &code);
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSemaphore(VkSemaphore *semaphore);
        void createSemaphores();
        void createFences();
        void checkSupportedInstanceExtensions();
        void checkAvailablePhysicalDevices();
        void checkValidationLayers();
        void checkPhysicalDevice(VkPhysicalDevice *device);
        std::vector<const char *> getRequiredInstanceExtensions();

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
        VkRenderPass renderPass;
        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderingFinishedSemaphore;
        VkSwapchainKHR swapchain;

        VulkanHandler(SDL_Window *sdlWindow, char *sdlWindowName);
        VulkanHandler(GLFWwindow *glfwWindow, char *glfwWindowName);

        void init();

        ~VulkanHandler();
};

#endif
