#pragma once
#include <volk.h>
#include "Vulkan_CommandBuffer.h"

inline uint32_t Buf_Func_FindSuitableMemoryType(VkPhysicalDevice device, uint32_t memoryTypeBits, VkMemoryPropertyFlags flags) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);//Получить характеристики памяти GPU

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
            return i;//Выбор индекса памяти в зависимости от доступа
        }
    }

    throw std::runtime_error("Unable to find suitable memory type");
}

//Создание буфера//
inline void Buf_Func_CreateBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size,
    VkBuffer& buffer, VkDeviceMemory& deviceMemory, VkBufferUsageFlags usage, VkMemoryPropertyFlags propertyFlags) {
    VkBufferCreateInfo bufferCreateInfo{};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.size = size; 
    bufferCreateInfo.usage = usage; 

    
    if(vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer)!=VK_SUCCESS) throw ERR_BUFFER_CREATION;

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex = Buf_Func_FindSuitableMemoryType(physicalDevice, memRequirements.memoryTypeBits, propertyFlags);

    if (vkAllocateMemory(device, &memAllocInfo, nullptr, &deviceMemory) != VK_SUCCESS) throw ERR_MEMORY_ALLOCATION;   
    vkBindBufferMemory(device, buffer, deviceMemory, 0);
}

//Скопировать из буфера в буфер//
inline void Buf_Func_CopyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue copyBufferQueue,
    VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize srcOffset, VkDeviceSize dstOffset, VkDeviceSize size) {

    CommandBuffer commandBuffer;
    commandBuffer.AllocateCommandBuffer(device, commandPool);
    commandBuffer.BeginCommandBuffer();

    
    VkBufferCopy bufferRegion{};
    {
        bufferRegion.srcOffset = srcOffset;
        bufferRegion.dstOffset = dstOffset;
        bufferRegion.size = size;
    }

    vkCmdCopyBuffer(commandBuffer.Get(), srcBuffer, dstBuffer, 1, &bufferRegion);
    

    commandBuffer.EndCommandBuffer();
    commandBuffer.SubmitCommandBuffer(copyBufferQueue);
    commandBuffer.FreeCommandBuffer(device, commandPool);
}

class UniformBuffer{
    VkBuffer	    m_Buffer;
    VkDeviceMemory  m_Memory;
public:
    void CreateUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice device, size_t size) {
        Buf_Func_CreateBuffer(physicalDevice, device, size, m_Buffer, m_Memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    void Destroy(VkDevice device) {
        vkFreeMemory(device, m_Memory, nullptr);
        vkDestroyBuffer(device, m_Buffer, nullptr);
    }
};

