#pragma once
#include "../DataTypes.h"
#include <vector>

#ifdef USE_GL
    #include <glad/glad.h>;
#endif

class VertexArray {
    #ifdef USE_GL
        GLuint m_vertexBuffer;
        GLuint m_indexBuffer;
        GLuint m_vertexArray;
    #endif

    #ifdef USE_VK
        VkBuffer m_VertexBuffer;    
        VkDeviceMemory m_VertexBufferDeviceMemory;

        VkBuffer m_IndexBuffer;
        VkDeviceMemory m_IndexBufferDeviceMemory;
    #endif

    std::vector<DataTypes::Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    public:

    #ifdef USE_GL
        GLuint GetVertexBuffer();
        GLuint GetIndexBuffer();
        GLuint GetVertexArray();
    #endif

    VertexArray(std::vector<DataTypes::Vertex> &vertices, std::vector<unsigned int> &indices);
    std::vector<unsigned int> GetIndices();
    ~VertexArray();
};




