#pragma once

#include "VertexArray.h"
#include "Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>

#ifdef USE_VK
    #include "../Vulkan_Implementation/Vulkan_BufferCreationFunctions.h"
#endif

using namespace DataTypes;

enum ShapeType {
    Triangle,
    Rectangle,
    Cube
};

class Mesh {
    std::vector<int> material_ID;
    //std::map<int, std::vector<Texture*>> dTextures;

    std::map<int, Texture*> dTextures;
    std::map<int, Texture*> sTextures;

    std::vector<VertexArray*> vertexArrays;

    void LoadMaterials(const aiScene* scene, std::string path);


    #ifdef USE_VK
        void InitUniformBuffers();
        void ClearUniformBuffers();

        void InitDescriptorSets();
        void ClearDescriptorSets();

        std::vector<VkDescriptorSet> descriptorSets;
    #endif
public:

    Mesh(ShapeType shapeType, std::string TexturePath);
    Mesh(ShapeType shapeType, glm::vec3 color);
    Mesh(std::string path);
    Mesh(std::string path, glm::vec3 color);

    #ifdef USE_GL
        void Draw();
    #endif

    #ifdef USE_VK
        UniformBuffer m_colorUniform;

        void Draw(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout Layout, int imageIndex);
    #endif

    ~Mesh();

};