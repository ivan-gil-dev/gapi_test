#include "../CoreClass.h"
#include "_Vulkan_Handles.h"
#include "Vulkan_CommandBuffer.h"
#ifdef USE_VK


std::vector<CommandBuffer> drawCommandBuffers;

int currentFrame = 0;

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
    VkFormat m_swapchainImageFormat;
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
                m_swapchainImageFormat = surfaceFormats[i].format;
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

        m_SwapchainImageViews.resize(m_SwapchainImages.size());
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

    VkFormat GetSwapchainImageFormat() {
        return m_swapchainImageFormat;
    }

    ~Swapchain() {
        
    }

}*localSwapchain;

class Framebuffer {
    std::vector<VkFramebuffer> m_framebuffers;
    public:
    Framebuffer(VkDevice device, std::vector<VkImageView> swapchainImageViews, VkRenderPass renderpass, int width, int height) {
        m_framebuffers.resize(swapchainImageViews.size());
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

class SyncObjects {
    std::vector<VkSemaphore> m_imagePrepareSemaphores;
    std::vector<VkSemaphore> m_imagePresentSemaphores;
    std::vector<VkFence> m_fences;
    public:
    SyncObjects(VkDevice device, unsigned int count) {
        m_imagePrepareSemaphores.resize(count);
        m_imagePresentSemaphores.resize(count);
        m_fences.resize(count);

        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (size_t i = 0; i < count; i++)
        {
            if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_imagePrepareSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_imagePresentSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(device, &fenceCreateInfo, nullptr, &m_fences[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create sync object");
         }
            
        
    }

    std::vector<VkSemaphore> GetImagePrepareSemaphores() {
        return m_imagePrepareSemaphores;
    }

    std::vector<VkSemaphore> GetImagePresentSemaphores() {
        return m_imagePresentSemaphores;
    }

    std::vector<VkFence> GetFences() {
        return m_fences;
    }

    void DestroySyncObjects(VkDevice device) {
        for (size_t i = 0; i < m_imagePresentSemaphores.size(); i++)
        {
            vkDestroySemaphore(device, m_imagePresentSemaphores[i], nullptr);
            vkDestroySemaphore(device, m_imagePrepareSemaphores[i], nullptr);
            vkDestroyFence(device, m_fences[i], nullptr);
        }
    }
}*localSyncObjects;


void FillExternHandles() {
    externInstance = localInstance->GetInstance();
    externDevice = localDevice->GetDevice();
    externPhysicalDevice = localDevice->GetPhysicalDevice();
    externSurface = localSurface->GetSurface();
    externSwapchain = localSwapchain->GetSwapchain();
}


void windowResizeCallback(GLFWwindow* window, int width, int height) {
    
}

CoreClass::CoreClass(WindowProperties properties) : m_properties(properties) {

    m_width = properties.width;
    m_height = properties.height;

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

    extern_MAX_FRAMES = localSwapchain->GetSwapchainImageViews().size();

    m_CommandPool = new CommandPool;
    m_CommandPool->CreateCommandPool(localDevice->GetQueueIndices().queueIndex1_graphics, localDevice->GetDevice());

    FillExternHandles();
    externCommandPool = m_CommandPool->Get();
   

    p_m_program = new Program(
        ProgramType::TEST_PIPELINE, 
        localSwapchain->GetSwapchainImageFormat(), 
        properties.width, properties.height
    );

    externPipeline = p_m_program->GetPipeline();
    externSetLayout = p_m_program->GetSetLayout();
    externDescriptorPool = p_m_program->GetDescriptorPool();
    
    localFramebuffer = new Framebuffer(
        localDevice->GetDevice(),
        localSwapchain->GetSwapchainImageViews(),
        p_m_program->GetRenderPass(),
        properties.width,
        properties.height
    );

    drawCommandBuffers.resize(extern_MAX_FRAMES);
    for (size_t i = 0; i < extern_MAX_FRAMES; i++)
        drawCommandBuffers[i].AllocateCommandBuffer(localDevice->GetDevice(), m_CommandPool->Get());


    localSyncObjects = new SyncObjects(localDevice->GetDevice(), extern_MAX_FRAMES);
}
CoreClass::~CoreClass() {

    for (size_t i = 0; i < extern_MAX_FRAMES; i++)
    {
        vkWaitForFences(localDevice->GetDevice(), 1, &localSyncObjects->GetFences()[i], VK_TRUE, UINT64_MAX);
    }
    for (size_t i = 0; i < extern_MAX_FRAMES; i++)
    {
        drawCommandBuffers[i].FreeCommandBuffer(localDevice->GetDevice(), m_CommandPool->Get());
    }

    delete m_sceneContainer;
    delete p_m_program;
    glfwDestroyWindow(p_m_window);
    glfwTerminate();

    localSwapchain->DestroySwapchain(localDevice->GetDevice());
    localSurface->DestroySurface(localInstance->GetInstance());
    m_CommandPool->Destroy(localDevice->GetDevice());
    localFramebuffer->DestroyFramebuffers(localDevice->GetDevice());
    localSyncObjects->DestroySyncObjects(localDevice->GetDevice());

    
    delete localSyncObjects;
    delete m_CommandPool;
    delete localSwapchain;
    delete localSurface;
    delete localDevice;
    delete localInstance;
    delete localFramebuffer;
}




void CoreClass::PrepareFrame(VkRenderPass mainRenderPass, std::vector<VkFramebuffer> frameBuffers) {
    vkWaitForFences(localDevice->GetDevice(), 1, &localSyncObjects->GetFences()[currentFrame], VK_TRUE, UINT64_MAX);
    
    VkResult result;
    result = vkAcquireNextImageKHR(
        localDevice->GetDevice(),
        localSwapchain->GetSwapchain(),
        UINT64_MAX,
        localSyncObjects->GetImagePrepareSemaphores()[currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    VkRenderPassBeginInfo renderPassBeginInfo;
    renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = mainRenderPass;
    renderPassBeginInfo.renderArea.extent.height = m_height;
    renderPassBeginInfo.renderArea.extent.width = m_width;
    renderPassBeginInfo.renderArea.offset = { 0,0 };
    renderPassBeginInfo.framebuffer = frameBuffers[imageIndex];

    
    VkClearValue clearColor = { m_clearColor.r,m_clearColor.g,m_clearColor.b,1.0f };

    VkClearValue clearColors[] = {
        clearColor
    };
    //VkClearValue depthClearColor = { 1.0,0.0f };

    renderPassBeginInfo.pClearValues = &clearColor;
    renderPassBeginInfo.clearValueCount = 1;
    
    drawCommandBuffers[currentFrame].BeginCommandBuffer();
    vkCmdBeginRenderPass(drawCommandBuffers[currentFrame].Get(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CoreClass::DrawFrame() {
    vkCmdEndRenderPass(drawCommandBuffers[currentFrame].Get());
    drawCommandBuffers[currentFrame].EndCommandBuffer();

    VkCommandBuffer commandBuffers[] = { drawCommandBuffers[currentFrame].Get() };
    VkPipelineStageFlags stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore waitSemaphores[] = { localSyncObjects->GetImagePrepareSemaphores()[currentFrame] };
    VkSemaphore signalSemaphores[] = { localSyncObjects->GetImagePresentSemaphores()[currentFrame] };

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = (uint32_t)1;
    submitInfo.pCommandBuffers = commandBuffers;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pWaitDstStageMask = stages;

    vkResetFences(localDevice->GetDevice(), 1, &localSyncObjects->GetFences()[currentFrame]);
    if (vkQueueSubmit(localDevice->GetMainQueue(), 1, &submitInfo, localSyncObjects->GetFences()[currentFrame]) != VK_SUCCESS)  throw ERR_QUEUE_SUBMITION;
    

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.waitSemaphoreCount = 1;

    VkSwapchainKHR swapchains[] = { localSwapchain->GetSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pResults = 0;
    presentInfo.pImageIndices = &imageIndex;

    
    VkResult result = vkQueuePresentKHR(localDevice->GetMainQueue(), &presentInfo); 

    vkDeviceWaitIdle(localDevice->GetDevice());
    currentFrame = (currentFrame + 1) % extern_MAX_FRAMES;
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
        }

        PrepareFrame(p_m_program->GetRenderPass(), localFramebuffer->GetFrameBuffers());
        for (size_t i = 0; i < m_sceneContainer->GetObjects()->size(); i++)
            m_sceneContainer->GetObjects()->at(i)->Draw(
                drawCommandBuffers[currentFrame].Get(),
                p_m_program->GetPipeline(),
                p_m_program->GetPipelineLayout(),
                imageIndex
            );
        DrawFrame();

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
    glm::vec3 color = glm::vec3(1,0,0);
    m_sceneContainer->GetObjects()->at(i)->p_m_mesh->m_colorUniform.UpdateBuffer(localDevice->GetDevice(), &color);
}


#endif