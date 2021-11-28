#include	<volk.h>
#include	<iostream>
#include "../ErrorCodes.h"
    //����� ��� �������� ������//
class CommandBuffer {
    VkCommandBuffer m_CommandBuffer;
public:
    //�������� ������ �� ���� ������//
    void AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        {
            allocInfo.commandBufferCount = 1;
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        }

        //�������� ��������� ����� �� ����
        if(vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer)!=VK_SUCCESS)
            throw ERR_COMMAND_BUFFER_ALLOCATION;
            
    }

    //������ ������ ������//
    void BeginCommandBuffer() {
        VkCommandBufferBeginInfo beginInfo = {};
        {
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        }


        if(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo)!=VK_SUCCESS)
            throw ERR_COMMAND_BUFFER_BEGIN;
            
    }

    //��������� ����� � �������//
    void SubmitCommandBuffer(VkQueue queue) {
        VkCommandBuffer pCommandBuffers[] = { m_CommandBuffer };
        VkSubmitInfo submitInfo{};
        {
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pCommandBuffers = pCommandBuffers;
            submitInfo.commandBufferCount = 1;
        }

        //��������� ����� � �������
        if(vkQueueSubmit(queue, 1, &submitInfo, nullptr)!=VK_SUCCESS) throw ERR_QUEUE_SUBMITION;
        
        vkQueueWaitIdle(queue);
    }

    //���������� ����� �� ������//
    void FreeCommandBuffer(VkDevice device, VkCommandPool commandPool) { //<���������� �����>
        if (m_CommandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device, commandPool, 1, &m_CommandBuffer);
            m_CommandBuffer = VK_NULL_HANDLE;
        }
    }

    //��������� ������//
    void EndCommandBuffer() {
        if(vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) throw ERR_COMMAND_BUFFER_END;
    }

public:
    VkCommandBuffer Get() {
        return m_CommandBuffer;
    }
};


