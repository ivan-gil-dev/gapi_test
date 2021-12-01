#include "../ErrorCodes.h"
#include <volk.h>

//Пул для команд используется для выделения памяти под буферы с командами//
class CommandPool {
    VkCommandPool m_CommandPool;
public:

    void CreateCommandPool(unsigned int queueIndex, VkDevice device) {
        VkCommandPoolCreateInfo commandPoolCreateInfo{}; {
            commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.queueFamilyIndex = queueIndex;
            commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        }

        if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
            throw ERR_COMMAND_POOL_CREATION;   
    }

    void Destroy(VkDevice device) {
        vkDestroyCommandPool(device, m_CommandPool, nullptr);
    }

public:
    VkCommandPool Get() {
        return m_CommandPool;
    }

    VkCommandPool* PGet() {
        return &m_CommandPool;
    }
};


