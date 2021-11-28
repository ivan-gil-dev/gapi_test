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
extern VkDescriptorSetLayout externSetLayout;
extern VkSurfaceKHR externSurface;
extern VkDescriptorPool externDescriptorPool;
extern VkFormat externSwapchainFormat;
extern VkFormat externDepthFormat;
extern VkCommandPool externCommandPool;
extern VkQueue externMainQueue;
extern unsigned int externMainQueueIndex;
