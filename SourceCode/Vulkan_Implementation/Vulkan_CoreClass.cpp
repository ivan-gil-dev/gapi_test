#include "../CoreClass.h"
#include "_Vulkan_Handles.h"
#ifdef USE_VK

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {

    if(messageType != VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT)
    std::cout << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

class Instance {
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_messenger;

    VkInstanceCreateInfo m_InstanceCreateInfo{};
    VkApplicationInfo m_AppInfo{};
    VkDebugUtilsMessengerCreateInfoEXT m_MessengerInfo{};

    public:
    Instance() {
        std::vector<const char*> layers = { "VK_LAYER_KHRONOS_validation" };
        m_AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        m_AppInfo.apiVersion = VK_MAKE_VERSION(1, 2, 0);
        m_AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        m_AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        m_AppInfo.pApplicationName = "API_TEST";
        m_AppInfo.pEngineName = "Engine";

        uint32_t extensionsCount;
        const char** p = glfwGetRequiredInstanceExtensions(&extensionsCount);
        std::vector<const char*> extensions(p, p + extensionsCount);
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        for (size_t i = 0; i < extensions.size(); i++)
            std::cout << extensions[i] << std::endl;

        m_InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        m_InstanceCreateInfo.enabledExtensionCount = extensions.size();
        m_InstanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        m_InstanceCreateInfo.pApplicationInfo = &m_AppInfo;
        m_InstanceCreateInfo.enabledLayerCount = (uint32_t)layers.size();
        m_InstanceCreateInfo.ppEnabledLayerNames = layers.data();

        m_MessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        m_MessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        m_MessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        m_MessengerInfo.pfnUserCallback = debugCallback;
        m_MessengerInfo.pUserData = nullptr;
        m_MessengerInfo.flags = 0;

        m_InstanceCreateInfo.pNext = &m_MessengerInfo;

        if (volkInitialize() != VK_SUCCESS) throw ERR_VOLK_INIT_FAIL;
        if (vkCreateInstance(&m_InstanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS) throw ERR_INSTANCE_CREATION;
        volkLoadInstance(m_instance);
        if (vkCreateDebugUtilsMessengerEXT(m_instance, &m_MessengerInfo, nullptr, &m_messenger)) throw ERR_DEBUG_UTILS_CREATION;
    }
    ~Instance() {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_messenger, nullptr);
        vkDestroyInstance(m_instance,nullptr);
    }
    VkInstance GetInstance(){return m_instance;}

}*localInstance;

class Device {
    VkPhysicalDevice m_physicalDevice;
    VkDevice m_device;
    VkQueueIndices m_queueIndices;
    VkQueue m_graphicsQueue;
    void PickPhysicalDevice(VkInstance instance) {
        uint32_t deviceCount;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (size_t i = 0; i < deviceCount; i++) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);

            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                std::cout << "Device :" << deviceProperties.deviceName << std::endl;
                m_physicalDevice = devices[i];
                break;
            }
        }


        uint32_t propertyCount;
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &propertyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueProperties(propertyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &propertyCount, queueProperties.data());

        bool suitableFlag = false;
        for (size_t i = 0; i < propertyCount; i++) 
            if (queueProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                m_queueIndices.queueIndex1_graphics = (uint32_t)i;
                externMainQueueIndex = VK_QUEUE_GRAPHICS_BIT;
                suitableFlag = true;
                break;
            }
        
        if (!suitableFlag) throw ERR_PHYSICAL_DEVICE_NOT_FOUND;
    }
    public:
    Device(VkInstance instance) {
        PickPhysicalDevice(instance);
        
        float priority = 1.0f;
        VkDeviceQueueCreateInfo deviceQueueCreateInfo{};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pQueuePriorities = &priority;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = m_queueIndices.queueIndex1_graphics;
        
        static std::vector<const char*> Layers = { "VK_LAYER_KHRONOS_validation" };
        static std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };


        VkDeviceCreateInfo deviceCreateInfo{};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
        deviceCreateInfo.queueCreateInfoCount = (uint32_t)1;
        deviceCreateInfo.ppEnabledLayerNames = Layers.data();
        deviceCreateInfo.enabledLayerCount = Layers.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();

        if(vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device)!=VK_SUCCESS)
            throw ERR_DEVICE_CREATION;

        vkGetDeviceQueue(m_device, m_queueIndices.queueIndex1_graphics, 0, &m_graphicsQueue);
        externMainQueue = m_graphicsQueue;
    }
    ~Device() {
        vkDestroyDevice(m_device,nullptr);
    }
    VkPhysicalDevice GetPhysicalDevice() {
        return m_physicalDevice;
    }
    VkQueue GetMainQueue() {
        return m_graphicsQueue;
    }
    VkDevice GetDevice() {
        return m_device;
    }
    VkQueueIndices GetQueueIndices() {
        return m_queueIndices;
    }
    
}*localDevice;

class Surface {
    VkSurfaceKHR m_surface;
    public:
    Surface(VkInstance instance, GLFWwindow* window) {
         if(glfwCreateWindowSurface(instance, window, nullptr, &m_surface) != VK_SUCCESS) throw ERR_SURFACE_CREATION;
    }

    VkSurfaceKHR GetSurface() {
        return m_surface;
    }

    void DestroySurface(VkInstance instance) {
        vkDestroySurfaceKHR(instance, m_surface, nullptr);
    }
    
    ~Surface() {
        
    }

}*localSurface;

class Swapchain {
    VkSwapchainKHR m_swapchain;
    std::vector<VkImage> m_SwapchainImages;
    std::vector<VkImageView> m_SwapchainImageViews;
    public:
    Swapchain(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, VkDevice device, int queueIndex) {
        VkSwapchainCreateInfoKHR CreateInfo{};

        VkBool32 swapchainSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface, &swapchainSupported);

        if (swapchainSupported != VK_TRUE) throw ERR_SWAPCHAIN_NOT_SUPPORTED;

        CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

        uint32_t presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, nullptr);
        std::vector<VkPresentModeKHR> presentModes(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModesCount, presentModes.data());

        uint32_t surfaceFormatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, nullptr);
        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &surfaceFormatCount, surfaceFormats.data());

        for (size_t i = 0; i < surfaceFormatCount; i++) {
            if (surfaceFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
                surfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                CreateInfo.imageColorSpace = surfaceFormats[i].colorSpace;
                CreateInfo.imageFormat = surfaceFormats[i].format;
                break;
            }
        }

        for (size_t i = 0; i < presentModesCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                CreateInfo.presentMode = presentModes[i];
                break;
            }
        }

        if (CreateInfo.presentMode != VK_PRESENT_MODE_MAILBOX_KHR) {
            CreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        CreateInfo.minImageCount = surfaceCapabilities.minImageCount + 1;
        CreateInfo.preTransform = surfaceCapabilities.currentTransform;

        CreateInfo.clipped = VK_FALSE;
        CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        CreateInfo.imageArrayLayers = 1;

        VkExtent2D extent = { surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height };

        CreateInfo.imageExtent = extent;
        CreateInfo.surface = surface;

        if (vkCreateSwapchainKHR(device, &CreateInfo, nullptr, &m_swapchain)) throw ERR_SWAPCHAIN_CREATION;
        
        uint32_t imagesCount;
        vkGetSwapchainImagesKHR(device, m_swapchain, &imagesCount, nullptr);
        m_SwapchainImages.resize(imagesCount);
        vkGetSwapchainImagesKHR(device, m_swapchain, &imagesCount, m_SwapchainImages.data());

        m_SwapchainImageViews.resize(m_SwapchainImageViews.size());
        for (size_t i = 0; i < m_SwapchainImageViews.size(); i++) {
            VkImageViewCreateInfo ImageViewCreateInfo{};
            ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ImageViewCreateInfo.format = CreateInfo.imageFormat;
            ImageViewCreateInfo.image = m_SwapchainImages[i];
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
            if (vkCreateImageView(device, &ImageViewCreateInfo, nullptr, &m_SwapchainImageViews[i]) != VK_SUCCESS) throw ERR_IMAGE_VIEW_CREATION;
        }

    }

    VkSwapchainKHR GetSwapchain() {
        return m_swapchain;
    }

    std::vector<VkImage> GetSwapchainImages() {
        return m_SwapchainImages;
    }

    std::vector<VkImageView> GetSwapchainImageViews() {
        return m_SwapchainImageViews;
    }

    void DestroySwapchain(VkDevice device) {
        for (size_t i = 0; i < m_SwapchainImageViews.size(); i++)
            vkDestroyImageView(device, m_SwapchainImageViews[i], nullptr);
       
        vkDestroySwapchainKHR(device, m_swapchain, nullptr);
    }

    ~Swapchain() {
        
    }

}*localSwapchain;

class Framebuffer {
    std::vector<VkFramebuffer> m_framebuffers;
    public:
    Framebuffer(VkDevice device, std::vector<VkImageView> swapchainImageViews, VkRenderPass renderpass, int width, int height) {
        for (size_t i = 0; i < swapchainImageViews.size(); i++) {
            VkImageView attachments[] = {
                swapchainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderpass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = width;
            framebufferInfo.height = height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) 
                throw ERR_FRAMEBUFFER_CREATION;
        }
            
    }
    
    void DestroyFramebuffers(VkDevice device) {
        for (size_t i = 0; i < m_framebuffers.size(); i++) vkDestroyFramebuffer(device, m_framebuffers[i], nullptr);
    }

    std::vector<VkFramebuffer> GetFrameBuffers() {
        return m_framebuffers;
    }

}*localFramebuffer;


void FillStaticHandles() {
    externInstance = localInstance->GetInstance();
    externDevice = localDevice->GetDevice();
    externPhysicalDevice = localDevice->GetPhysicalDevice();
    externSurface = localSurface->GetSurface();
    externSwapchain = localSwapchain->GetSwapchain();
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {
    
}

CoreClass::CoreClass(WindowProperties properties) : m_properties(properties) {
    if (!glfwInit()) throw ERR_GLFW_INIT_FAIL;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    p_m_window = glfwCreateWindow(m_properties.width, m_properties.height, m_properties.title, nullptr, nullptr);
    m_clearColor = glm::vec3(0.2, 0.2, 0.2);
    glfwSetWindowSizeCallback(p_m_window, windowResizeCallback);

    localInstance = new Instance(); 
    localDevice = new Device(localInstance->GetInstance());
    localSurface = new Surface(localInstance->GetInstance(), p_m_window);

    localSwapchain = new Swapchain(
        localSurface->GetSurface(),
        localDevice->GetPhysicalDevice(),
        localDevice->GetDevice(),
        localDevice->GetQueueIndices().queueIndex1_graphics
    );

    

    m_CommandPool = new CommandPool;
    m_CommandPool->CreateCommandPool(localDevice->GetQueueIndices().queueIndex1_graphics, localDevice->GetDevice());

    FillStaticHandles();
    externCommandPool = m_CommandPool->Get();
   

    p_m_program = new Program(ProgramType::MAIN_PIPELINE);

    localFramebuffer = new Framebuffer(
        localDevice->GetDevice(),
        localSwapchain->GetSwapchainImageViews(),
        p_m_program->GetRenderPass(),
        1366,
        768
    );

    


}
CoreClass::~CoreClass() {
    delete m_sceneContainer;
    delete p_m_program;
    glfwDestroyWindow(p_m_window);
    glfwTerminate();

    localSwapchain->DestroySwapchain(localDevice->GetDevice());
    localSurface->DestroySurface(localInstance->GetInstance());
    m_CommandPool->Destroy(localDevice->GetDevice());
    localFramebuffer->DestroyFramebuffers(localDevice->GetDevice());

    delete m_CommandPool;
    delete localSwapchain;
    delete localSurface;
    delete localDevice;
    delete localInstance;
    delete localFramebuffer;
}


void CoreClass::PrepareFrame() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);
}

void CoreClass::DrawFrame() {
    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}


void CoreClass::Play(SceneContainer* sceneContainer) {
    m_sceneContainer = sceneContainer;
    while (!glfwWindowShouldClose(p_m_window))//Пока окно не закрыто
    {
        auto beginTime = Timer.now();//Получаем время начала итерации цикла

        //Выставить значение состояния текущей используемой программы
        p_m_program->UseProgram();
        m_camera.Update(p_m_window, DeltaTime);





        for (size_t i = 0; i < m_sceneContainer->GetObjects()->size(); i++)
        {
            m_sceneContainer->GetObjects()->at(i)->Update(DeltaTime);
            UpdateUniformsForObject(i);
            m_sceneContainer->GetObjects()->at(i)->Draw();   
        }

        //Обработать оконные события
        glfwPollEvents();

        auto endTime = Timer.now();//Получаем время конца итерации цикла

        //Вычисляем разность между временем начала и конца итерации цикла
        DeltaTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count();
        //Перевод в секунды
        DeltaTime /= 1000000;
    }
}
void CoreClass::UpdateUniformsForObject(int i) {

}


#endif