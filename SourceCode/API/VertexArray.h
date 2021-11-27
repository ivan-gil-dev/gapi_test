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

    std::vector<DataTypes::Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    public:

    VertexArray(std::vector<DataTypes::Vertex> vertices, std::vector<unsigned int> indices);

    #ifdef USE_GL
        GLuint GetVertexBuffer();
        GLuint GetIndexBuffer();
        GLuint GetVertexArray();
    #endif

    std::vector<unsigned int> GetIndices();

    ~VertexArray();
};



