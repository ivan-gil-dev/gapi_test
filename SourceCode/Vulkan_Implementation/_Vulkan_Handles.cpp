#include "_Vulkan_Handles.h"

VkInstance externInstance;
VkDevice externDevice;
VkPhysicalDevice externPhysicalDevice;
VkSwapchainKHR externSwapchain;
VkPipeline externPipeline;
VkSurfaceKHR externSurface;
VkFormat externSwapchainFormat = VK_FORMAT_B8G8R8A8_SRGB;
VkFormat externDepthFormat = VK_FORMAT_D32_SFLOAT;
unsigned int externMainQueueIndex;
VkQueue externMainQueue;
VkCommandPool externCommandPool;
int extern_MAX_FRAMES;


VkDescriptorPool externDescriptorPool_uniforms;
VkDescriptorSetLayout externSetLayout_uniforms;
VkDescriptorPool externDescriptorPool_samplers;
VkDescriptorSetLayout externSetLayout_samplers;