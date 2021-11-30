#pragma once
#include <volk.h>
#include "Vulkan_CommandBuffer.h"
#include "_Vulkan_Handles.h"
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
    VkDescriptorBufferInfo m_Descriptor;
public:
    void CreateUniformBuffer(VkPhysicalDevice physicalDevice, VkDevice device, size_t size) {
        Buf_Func_CreateBuffer(physicalDevice, device, size, m_Buffer, m_Memory, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        m_Descriptor.buffer = m_Buffer;
        m_Descriptor.offset = 0;
        m_Descriptor.range = size;
    }

    void UpdateBuffer(VkDevice device, void* data) {
        void *pointer;

        vkMapMemory(device, m_Memory, 0, m_Descriptor.range, 0, &pointer);
        memcpy(pointer, data, m_Descriptor.range);
        vkUnmapMemory(device, m_Memory);
    }

    VkDescriptorBufferInfo* GetDescriptor() {
        return &m_Descriptor;
    }

    void Destroy(VkDevice device) {
        vkFreeMemory(device, m_Memory, nullptr);
        vkDestroyBuffer(device, m_Buffer, nullptr);
    }
};

//Создание изображения//
inline void Img_Func_CreateImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImage& image,
    VkDeviceMemory& imageTextureMemory, uint32_t width, uint32_t height, VkImageTiling tiling,
    VkMemoryPropertyFlags properties, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits samples,
    uint32_t arrayLayers, VkImageCreateFlags flags) {
    //Ширина, высота изображения//
    VkExtent3D extent = { width,height,1 };

    //Заполнение структуры создания//
    VkImageCreateInfo imageCreateInfo = {};
    {
        imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageCreateInfo.pNext = nullptr;
        imageCreateInfo.flags = flags;
        imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;//2d изображение//
        imageCreateInfo.format = format;//формат изображения//
        imageCreateInfo.extent = extent;//размеры//
        imageCreateInfo.mipLevels = (uint32_t)1;
        imageCreateInfo.arrayLayers = arrayLayers;//сколько изображений если это массив//
        imageCreateInfo.samples = samples;////
        imageCreateInfo.tiling = tiling;
        imageCreateInfo.usage = usage;
        imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageCreateInfo.queueFamilyIndexCount = (uint32_t)0;
        imageCreateInfo.pQueueFamilyIndices = VK_NULL_HANDLE;
        imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    }

    if (vkCreateImage(device, &imageCreateInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Image");
    }

    //После создания изображения нужно привязать память к изображению//

    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo allocateInfo{};

    {
        vkGetImageMemoryRequirements(device, image, &memRequirements);
        allocateInfo.allocationSize = memRequirements.size;
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.memoryTypeIndex = Buf_Func_FindSuitableMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);
    }

    VkResult result = vkAllocateMemory(device, &allocateInfo, nullptr, &imageTextureMemory);
    if (result != VK_SUCCESS) {
        std::cout << "Error code " << result << std::endl;
        throw std::runtime_error("Failed to allocate image memory");
    }

    vkBindImageMemory(device, image, imageTextureMemory, 0);
}

//Перевод изображения из одного слоя в другой//
inline void Img_Func_TransitionImageLayout(VkDevice device, VkQueue pipelineBarrierQueue,
    VkCommandPool commandPool, VkImage image, VkImageLayout oldLayout,
    VkImageLayout newLayout, VkImageSubresourceRange subresourceRange) {
    CommandBuffer commandBuffer;
    commandBuffer.AllocateCommandBuffer(device, commandPool);
    commandBuffer.BeginCommandBuffer();

    VkImageMemoryBarrier
        memoryBarrier{};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.oldLayout = oldLayout;
    memoryBarrier.newLayout = newLayout;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.image = image;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags dstStage;

    //Из неопределенного слоя в слой передачи//
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        memoryBarrier.srcAccessMask = 0;
        memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }

    //Из слоя передачи в слой чтения из шейдера//
    if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
        newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    //Из неопределенного слоя в слой Z-буфера//
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
        newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        memoryBarrier.srcAccessMask = 0;
        memoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }

    memoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(commandBuffer.Get(),
        sourceStage, dstStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &memoryBarrier
    );

    commandBuffer.EndCommandBuffer();
    commandBuffer.SubmitCommandBuffer(pipelineBarrierQueue);
    commandBuffer.FreeCommandBuffer(device, commandPool);
}

//Копировать из буфера в изображение//
inline void Img_Func_CopyBufferToImage(VkDevice device, VkQueue copyBufferQueue,
    VkCommandPool commandPool, VkImage image, VkBuffer buffer, uint32_t width, uint32_t height) {
    CommandBuffer commandBuffer;
    commandBuffer.AllocateCommandBuffer(device, commandPool);
    commandBuffer.BeginCommandBuffer();

    //Размеры изображения//
    VkBufferImageCopy bufferImageCopy{};
    bufferImageCopy.imageExtent.width = width;
    bufferImageCopy.imageExtent.height = height;
    bufferImageCopy.imageExtent.depth = 1;
    bufferImageCopy.bufferOffset = 0;
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;
    bufferImageCopy.imageOffset = { 0,0,0 };

    //количество изображений в буфере//
    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    
    
    vkCmdCopyBufferToImage(commandBuffer.Get(), buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

    commandBuffer.EndCommandBuffer();
    commandBuffer.SubmitCommandBuffer(copyBufferQueue);
    commandBuffer.FreeCommandBuffer(device, commandPool);
}

//Копировать из буфера (в котором 6 изображений) в изображение
inline void Img_func_CopyBufferToCubemap(VkDevice device, VkQueue copyBufferQueue, VkCommandPool commandPool,
    VkImage image, VkBuffer buffer, uint32_t width, uint32_t height) {
    CommandBuffer commandBuffer;
    commandBuffer.AllocateCommandBuffer(device, commandPool);
    commandBuffer.BeginCommandBuffer();

    std::vector<VkBufferImageCopy> bufferImageCopyVector(6);

    for (uint32_t i = 0; i < bufferImageCopyVector.size(); i++) {
        VkBufferImageCopy bufferImageCopy{};
        bufferImageCopy.imageExtent.width = width;
        bufferImageCopy.imageExtent.height = height;
        bufferImageCopy.imageExtent.depth = 1;
        //сдвиг в буфере на одно изображение//
        bufferImageCopy.bufferOffset = i * width * height * 4;
        bufferImageCopy.bufferRowLength = 0;
        bufferImageCopy.bufferImageHeight = 0;
        bufferImageCopy.imageOffset = { 0,0,0 };

        bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        //i-й номер в массиве//
        bufferImageCopy.imageSubresource.baseArrayLayer = i;
        bufferImageCopy.imageSubresource.layerCount = 1;
        bufferImageCopy.imageSubresource.mipLevel = 0;

        bufferImageCopyVector[i] = bufferImageCopy;
    }

    vkCmdCopyBufferToImage(
        commandBuffer.Get(),
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        (uint32_t)bufferImageCopyVector.size(),
        bufferImageCopyVector.data()
    );

    commandBuffer.EndCommandBuffer();
    commandBuffer.SubmitCommandBuffer(copyBufferQueue);
    commandBuffer.FreeCommandBuffer(device, commandPool);
}

class DepthImage {
    VkImage			vDepthImage;
    VkDeviceMemory  DepthImageMemory;
    VkImageView		DepthImageView;
    VkFormat		DepthFormat;

    void CreateDepthImageView(VkDevice device) {
        VkImageViewCreateInfo ImageViewCreateInfo{};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.format = DepthFormat;
        ImageViewCreateInfo.image = vDepthImage;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        //ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        if (vkCreateImageView(device, &ImageViewCreateInfo, nullptr, &DepthImageView) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create Image View");
        }
    }
    public:
    //Создание Z-буфера//
    void CreateDepthBuffer(VkDevice logicalDevice, VkQueue commandBufferQueue,
        VkExtent2D swapchainExtent, VkPhysicalDevice physicalDevice, VkCommandPool commandPool) {
        //Точность Z-буфера//
        DepthFormat = VK_FORMAT_D32_SFLOAT;

        //Создание изображения для Z-буфера//
        Img_Func_CreateImage(
            physicalDevice,
            logicalDevice,
            vDepthImage,
            DepthImageMemory,
            swapchainExtent.width,
            swapchainExtent.height,
            VK_IMAGE_TILING_OPTIMAL,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            DepthFormat,
            //Используется как Z-буфер//
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            VK_SAMPLE_COUNT_1_BIT,
            (uint32_t)1,
            NULL
        );

        CreateDepthImageView(logicalDevice);

        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.layerCount = 1;
        subresourceRange.levelCount = 1;

        //Перевод в слой для z-буфера//
        Img_Func_TransitionImageLayout(
            logicalDevice,
            commandBufferQueue,
            commandPool,
            vDepthImage,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            subresourceRange
        );
    }

    void Destroy(VkDevice device) {
        vkDestroyImage(device, vDepthImage, nullptr);
        vkFreeMemory(device, DepthImageMemory, nullptr);
        vkDestroyImageView(device, DepthImageView, nullptr);
    }

    VkFormat GetDepthFormat() {
        return DepthFormat;
    }

    VkImage GetDepthImage() {
        return vDepthImage;
    }

    VkImageView GetImageView() {
        return DepthImageView;
    }

};

