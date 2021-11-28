#include	<volk.h>
#include	<iostream>
#include "../ErrorCodes.h"
    //Буфер для хранения команд//
class CommandBuffer {
    VkCommandBuffer m_CommandBuffer;
public:
    //Выделить память из пула команд//
    void AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool) {
        VkCommandBufferAllocateInfo allocInfo{};
        {
            allocInfo.commandBufferCount = 1;
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = commandPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        }

        //Выделить командный буфер из пула
        if(vkAllocateCommandBuffers(device, &allocInfo, &m_CommandBuffer)!=VK_SUCCESS)
            throw ERR_COMMAND_BUFFER_ALLOCATION;
            
    }

    //Начать запись команд//
    void BeginCommandBuffer() {
        VkCommandBufferBeginInfo beginInfo = {};
        {
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        }


        if(vkBeginCommandBuffer(m_CommandBuffer, &beginInfo)!=VK_SUCCESS)
            throw ERR_COMMAND_BUFFER_BEGIN;
            
    }

    //Отправить буфер в очередь//
    void SubmitCommandBuffer(VkQueue queue) {
        VkCommandBuffer pCommandBuffers[] = { m_CommandBuffer };
        VkSubmitInfo submitInfo{};
        {
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.pCommandBuffers = pCommandBuffers;
            submitInfo.commandBufferCount = 1;
        }

        //Отправить буфер в очередь
        if(vkQueueSubmit(queue, 1, &submitInfo, nullptr)!=VK_SUCCESS) throw ERR_QUEUE_SUBMITION;
        
        vkQueueWaitIdle(queue);
    }

    //Освободить буфер от команд//
    void FreeCommandBuffer(VkDevice device, VkCommandPool commandPool) { //<Освободить буфер>
        if (m_CommandBuffer != VK_NULL_HANDLE)
        {
            vkFreeCommandBuffers(device, commandPool, 1, &m_CommandBuffer);
            m_CommandBuffer = VK_NULL_HANDLE;
        }
    }

    //Закончить запись//
    void EndCommandBuffer() {
        if(vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) throw ERR_COMMAND_BUFFER_END;
    }

public:
    VkCommandBuffer Get() {
        return m_CommandBuffer;
    }
};


