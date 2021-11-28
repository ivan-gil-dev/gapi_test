#pragma once
enum errors {
    ERR_GLFW_INIT_FAIL =  0x100,
    ERR_GLAD_INIT_FAIL,
    ERR_VOLK_INIT_FAIL,
    ERR_INSTANCE_CREATION, 
    ERR_DEBUG_UTILS_CREATION,
    ERR_DEVICE_CREATION,
    ERR_BUFFER_CREATION,
    ERR_PHYSICAL_DEVICE_NOT_FOUND,
    ERR_SURFACE_CREATION,
    ERR_SWAPCHAIN_CREATION,
    ERR_SWAPCHAIN_NOT_SUPPORTED,
    ERR_IMAGE_VIEW_CREATION,
    ERR_RENDERPASS_CREATION,
    ERR_SHADER_SRC_NOT_FOUND,
    ERR_PIPELINE_CREATION,
    ERR_SHADER_MODULE_CREATION,
    ERR_PIPELINE_LAYOUT_CREATION,
    ERR_SET_LAYOUT_CREATION,
    ERR_COMMAND_BUFFER_ALLOCATION,
    ERR_COMMAND_POOL_CREATION,
    ERR_COMMAND_BUFFER_BEGIN,
    ERR_COMMAND_BUFFER_END,
    ERR_QUEUE_SUBMITION,    
    ERR_MEMORY_ALLOCATION,
    ERR_FRAMEBUFFER_CREATION
};
