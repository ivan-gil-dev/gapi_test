#include "../API/VertexArray.h"
#include "Vulkan_BufferCreationFunctions.h"
#include "_Vulkan_Handles.h"
#ifdef USE_VK

VkBuffer VertexArray::GetIndexBuffer() {
    return m_IndexBuffer;
}

VkBuffer VertexArray::GetVertexBuffer() {
    return m_VertexBuffer;
}


VertexArray::VertexArray(std::vector<DataTypes::Vertex> &vertices, std::vector<unsigned int> &indices){
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    VkDeviceSize VertexBufferSize = sizeof(vertices[0]) * vertices.size();
    VkDeviceSize IndexBufferSize = sizeof(indices[0]) * indices.size();

    //---vertex-buffer----------------------------------------------------------------------
    //�������� �������������� ������ � ������ �����//
    //����� �������� ��� ������//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        VertexBufferSize,
        stagingBuffer,
        stagingMemory,
        //��� ������: �������� ��������
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //������� � ��������� ��� ������ ������ ����� (RAM ��� SRAM)
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    //������ ������ � ������������� �����//
    void* data;
    vkMapMemory(externDevice, stagingMemory, 0, VertexBufferSize, 0, &data);
    memcpy(data, vertices.data(), VertexBufferSize);
    vkUnmapMemory(externDevice, stagingMemory);

    //�������� ������ � ������ GPU ������������ ��� ������ �� �����//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        VertexBufferSize,
        m_VertexBuffer,
        m_VertexBufferDeviceMemory,
        //��� ������: �������� ��������, ��������� �����//
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        //��������� ������ GPU//
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    //����������� ������ �� ������ � ������ �����//
    //� ����� � ������ GPU//
    Buf_Func_CopyBuffer(
        externDevice,
        externCommandPool,
        externMainQueue,
        stagingBuffer,
        m_VertexBuffer,
        0,
        0,
        VertexBufferSize
    );

    vkDestroyBuffer(externDevice, stagingBuffer, nullptr);
    vkFreeMemory(externDevice, stagingMemory, nullptr);
    //--------------------------------------------------------------------------------------

    //---index-buffer-----------------------------------------------------------------------
    //�������� �������������� ������ � ������ �����//
    //����� �������� ��� ������//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        IndexBufferSize,
        stagingBuffer,
        stagingMemory,
        //��� ������: �������� ��������
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        //������� � ��������� ��� ������ ������ ����� (RAM ��� SRAM)
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    //������ ������ � ������������� �����//
    vkMapMemory(externDevice, stagingMemory, 0, IndexBufferSize, 0, &data);
    memcpy(data, indices.data(), IndexBufferSize);
    vkUnmapMemory(externDevice, stagingMemory);

    //�������� ������ � ������ GPU ������������ ��� ������ �� �����//
    Buf_Func_CreateBuffer(
        externPhysicalDevice,
        externDevice,
        IndexBufferSize,
        m_IndexBuffer,
        m_IndexBufferDeviceMemory,
        //��� ������: �������� ��������, ��������� �����//
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        //��������� ������ GPU//
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    //����������� ������ �� ������ � ������ �����//
    //� ����� � ������ GPU//
    Buf_Func_CopyBuffer(
        externDevice,
        externCommandPool,
        externMainQueue,
        stagingBuffer,
        m_IndexBuffer,
        0,
        0,
        IndexBufferSize
    );

    vkDestroyBuffer(externDevice, stagingBuffer, nullptr);
    vkFreeMemory(externDevice, stagingMemory, nullptr);
    //--------------------------------------------------------------------------------------

    m_vertices = vertices;
    m_indices = indices;
}

std::vector<unsigned int> VertexArray::GetIndices(){
    
    return m_indices;
}

VertexArray::~VertexArray(){
    vkFreeMemory(externDevice, m_VertexBufferDeviceMemory, nullptr);
    vkDestroyBuffer(externDevice, m_VertexBuffer, nullptr);
    vkFreeMemory(externDevice, m_IndexBufferDeviceMemory, nullptr);
    vkDestroyBuffer(externDevice, m_IndexBuffer, nullptr);
}

#endif