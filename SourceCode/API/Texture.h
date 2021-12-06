#pragma once

#include "../DataTypes.h"
#include <vector>
#include <string>

#include <iostream>
#ifdef USE_GL
#include <glad/glad.h>;
#endif

class Texture {

#ifdef USE_GL
    GLuint  texture;
#endif

#ifdef USE_VK
    uint32_t mipLevels;
    VkImage m_Image;
    VkImageView m_ImageView;
    VkSampler m_Sampler;
    VkDeviceMemory m_ImageTextureMemory;
    VkDescriptorImageInfo m_Descriptor{};
    void CreateImageView(VkDevice device);
    void CreateSampler(VkDevice device);
    void GenerateMipMaps(VkDevice device, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
#endif

    
public:

    #ifdef USE_VK
    VkDescriptorImageInfo* GetDescriptor();
        
    #endif

    Texture(std::string path);

    Texture(glm::vec3 color);

    void BindTexture(unsigned int textureSlot);

    ~Texture();

};