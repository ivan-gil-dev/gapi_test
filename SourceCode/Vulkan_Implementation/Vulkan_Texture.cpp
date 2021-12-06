#include "../API/Texture.h"
#include "_Vulkan_Handles.h"
#include "Vulkan_BufferCreationFunctions.h"

#ifdef USE_VK
#define STB_IMAGE_IMPLEMENTATION  
#include <stb_image.h>

void Texture::GenerateMipMaps(VkDevice device, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    CommandBuffer commandBuffer;
    commandBuffer.AllocateCommandBuffer(externDevice, externCommandPool);
    commandBuffer.BeginCommandBuffer();

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer.Get(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer.Get(),
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer.Get(),

            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }


    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer.Get(),
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    commandBuffer.EndCommandBuffer();
    commandBuffer.SubmitCommandBuffer(externMainQueue);
    commandBuffer.FreeCommandBuffer(externDevice, externCommandPool);
    
    


    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(externPhysicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }
}

VkDescriptorImageInfo* Texture::GetDescriptor() {
    return &m_Descriptor;
}

void Texture::CreateImageView(VkDevice device) {
    VkImageViewCreateInfo ImageViewCreateInfo{};
    ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    ImageViewCreateInfo.image = m_Image;
    ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    ImageViewCreateInfo.subresourceRange.levelCount = mipLevels;
    ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    ImageViewCreateInfo.subresourceRange.layerCount = 1;
    ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    if (vkCreateImageView(device, &ImageViewCreateInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Image View");
    }
}

void Texture::CreateSampler(VkDevice device) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.maxLod = mipLevels;     
    samplerInfo.minLod = 1.0f;
    samplerInfo.mipLodBias = 0.0f;

    if (vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }
}

Texture::Texture(std::string path){
    bool Failed = false;
    int texWidth, texHeight, texChannels;
    //Загрузка изображения из диска//
    unsigned char *Pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!Pixels) {
        Pixels = new unsigned char[3];
        Pixels[0] = 252;
        Pixels[1] = 2;
        Pixels[2] = 252;
        texWidth = 1;
        texHeight = 1;
        Failed = true;
        std::cout << "Failed to load Image" << std::endl;
    }
    
    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    //В одном пикселе 4 байта -> Размер изображения = ширина * высота * 4 байт
    VkDeviceSize imageSize = texHeight * texWidth * 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory bufferMemory;

    //Создание промежуточного буфера//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        imageSize,
        stagingBuffer,
        bufferMemory,
        //Буфер для передачи//
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //Выделение памяти из хоста//
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    void* data;
    vkMapMemory(externDevice, bufferMemory, 0, imageSize, 0, &data);
    memcpy(data, Pixels, (size_t)imageSize);
    vkUnmapMemory(externDevice, bufferMemory);


    VkFormat ImageFormat = VK_FORMAT_R8G8B8A8_SRGB;


    //Создание изображения//
    Img_Func_CreateImage(
        externPhysicalDevice,
        externDevice,
        m_Image,
        m_ImageTextureMemory,
        texWidth,
        texHeight,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        ImageFormat,
        //Изображение используется для приема данных и для отрисовки//
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_SAMPLE_COUNT_1_BIT,
        (uint32_t)1,
        NULL,
        mipLevels
    );

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.layerCount = 1;
    subresourceRange.levelCount = mipLevels;

    //Перевод изображения в слой для получения данных//
    Img_Func_TransitionImageLayout(
        externDevice,
        externMainQueue,
        externCommandPool,
        m_Image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresourceRange
    );

    //Копирование данных из буфера в изображение//
    Img_Func_CopyBufferToImage(
        externDevice,
        externMainQueue,
        externCommandPool,
        m_Image,
        stagingBuffer,
        texWidth,
        texHeight
    );

    ////Перевод изображения в слой для чтения из шейдера//
    //Img_Func_TransitionImageLayout(
    //    externDevice,
    //    externMainQueue,
    //    externCommandPool,
    //    m_Image,
    //    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    //    subresourceRange
    //);
    GenerateMipMaps(externDevice, m_Image, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);


    vkDestroyBuffer(externDevice, stagingBuffer, nullptr);
    vkFreeMemory(externDevice, bufferMemory, nullptr);

    CreateImageView(externDevice);
    CreateSampler(externDevice);

    m_Descriptor.imageView = m_ImageView;
    m_Descriptor.sampler = m_Sampler;
    m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    
    if (Failed) {
        delete []Pixels;
    }
    else {
        stbi_image_free(Pixels);
    }

}

Texture::Texture(glm::vec3 color){
    int texWidth = 1, texHeight = 1;
    unsigned char Pixels[] = {(unsigned char)color.x, (unsigned char)color.y, (unsigned char)color.z, 255};

    mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    //В одном пикселе 4 байта -> Размер изображения = ширина * высота * 4 байт
    VkDeviceSize imageSize = 4;
    VkBuffer stagingBuffer;
    VkDeviceMemory bufferMemory;

    //Создание промежуточного буфера//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        imageSize,
        stagingBuffer,
        bufferMemory,
        //Буфер для передачи//
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //Выделение памяти из хоста//
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
    );

    void* data;
    vkMapMemory(externDevice, bufferMemory, 0, imageSize, 0, &data);
    memcpy(data, Pixels, (size_t)imageSize);
    vkUnmapMemory(externDevice, bufferMemory);


    //Создание изображения//
    Img_Func_CreateImage(
        externPhysicalDevice,
        externDevice,
        m_Image,
        m_ImageTextureMemory,
        texWidth,
        texHeight,
        VK_IMAGE_TILING_OPTIMAL,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        VK_FORMAT_R8G8B8A8_SRGB,
        //Изображение используется для приема данных и для отрисовки//
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_SAMPLE_COUNT_1_BIT,
        (uint32_t)1,
        NULL,
        mipLevels
    );

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.baseMipLevel = 0;
    subresourceRange.layerCount = 1;
    subresourceRange.levelCount = 1;

    //Перевод изображения в слой для получения данных//
    Img_Func_TransitionImageLayout(
        externDevice,
        externMainQueue,
        externCommandPool,
        m_Image,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        subresourceRange
    );

    //Копирование данных из буфера в изображение//
    Img_Func_CopyBufferToImage(
        externDevice,
        externMainQueue,
        externCommandPool,
        m_Image,
        stagingBuffer,
        texWidth,
        texHeight
    );

    //Перевод изображения в слой для чтения из шейдера//
    Img_Func_TransitionImageLayout(
        externDevice,
        externMainQueue,
        externCommandPool,
        m_Image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        subresourceRange
    );

    vkDestroyBuffer(externDevice, stagingBuffer, nullptr);
    vkFreeMemory(externDevice, bufferMemory, nullptr);

    CreateImageView(externDevice);
    CreateSampler(externDevice);

    m_Descriptor.imageView = m_ImageView;
    m_Descriptor.sampler = m_Sampler;
    m_Descriptor.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

void Texture::BindTexture(unsigned int textureSlot){}

Texture::~Texture(){
    vkDestroySampler(externDevice, m_Sampler, nullptr);
    vkDestroyImage(externDevice, m_Image, nullptr);
    vkDestroyImageView(externDevice, m_ImageView, nullptr);
    vkFreeMemory(externDevice, m_ImageTextureMemory, nullptr);
}


#endif