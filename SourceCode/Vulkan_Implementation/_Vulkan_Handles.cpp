#include "_Vulkan_Handles.h"

VkInstance externInstance;
VkDevice externDevice;
VkPhysicalDevice externPhysicalDevice;
VkSwapchainKHR externSwapchain;
VkPipeline externPipeline;
VkDescriptorSetLayout externSetLayout;
VkSurfaceKHR externSurface;
VkDescriptorPool externDescriptorPool;
VkFormat externSwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
VkFormat externDepthFormat = VK_FORMAT_D16_UNORM;
unsigned int externMainQueueIndex;
VkQueue externMainQueue;
VkCommandPool externCommandPool;
int extern_MAX_FRAMES;