#include "../API/Texture.h"
#include "_Vulkan_Handles.h"
#include "Vulkan_BufferCreationFunctions.h"

#ifdef USE_VK
#define STB_IMAGE_IMPLEMENTATION  
#include <stb_image.h>


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
    ImageViewCreateInfo.subresourceRange.levelCount = 1;
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
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

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
        NULL
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
        NULL
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