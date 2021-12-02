#pragma once
#include <volk.h>

//
// Values being written to these variables only from CoreClass.cpp
//
extern VkInstance externInstance;
extern VkDevice externDevice;
extern VkPhysicalDevice externPhysicalDevice;
extern VkSwapchainKHR externSwapchain;
extern VkPipeline externPipeline;

extern VkSurfaceKHR externSurface;

extern VkFormat externSwapchainFormat;
extern VkFormat externDepthFormat;
extern VkCommandPool externCommandPool;
extern VkQueue externMainQueue;


extern unsigned int externMainQueueIndex;
extern int extern_MAX_FRAMES;
extern int extern_Swapchain_Image_View_Count;

extern VkPhysicalDeviceProperties externDeviceProperties;

extern VkDescriptorPool externDescriptorPool_uniforms;
extern VkDescriptorSetLayout externSetLayout_uniforms;

extern VkDescriptorPool externDescriptorPool_samplers;
extern VkDescriptorSetLayout externSetLayout_samplers;