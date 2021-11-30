#include "../API/Program.h"
#include "_Vulkan_Handles.h"

#include <fstream>
#ifdef USE_VK

VkPipeline Program::GetPipeline() {
    return m_pipeline;
}

std::vector<char> ReadShader(std::string path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw ERR_SHADER_SRC_NOT_FOUND;
    }
    uint32_t size = (uint32_t)file.tellg();
    std::vector<char> shader(size);
    file.seekg(0);
    file.read(shader.data(), size);
    file.close();
    return shader;
}

VkShaderModule Program::CreateShaderModule(VkDevice device, std::vector<char>& code) {
    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.codeSize = code.size() ;
    shaderModuleInfo.pCode = (uint32_t*)code.data();
    shaderModuleInfo.sType = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };

    VkResult result = vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule);
    if (result != VK_SUCCESS) {
        std::cout << "Result: " << result << std::endl;;
        throw ERR_SHADER_MODULE_CREATION;
    }

    return shaderModule;
}

void Program::CreateDescriptorSetLayout(ProgramType programType) {
    if (programType == ProgramType::MAIN_PIPELINE) {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;

        VkDescriptorSetLayoutBinding mvpBinding{};
        mvpBinding.binding = 0;
        mvpBinding.descriptorCount = 1;
        mvpBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        setLayoutBindings.push_back(mvpBinding);

        VkDescriptorSetLayoutBinding pointLightDataBinding{};
        pointLightDataBinding.binding = 1;
        pointLightDataBinding.descriptorCount = MAX_POINTLIGHT_COUNT;
        pointLightDataBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pointLightDataBinding.pImmutableSamplers = nullptr;
        pointLightDataBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(pointLightDataBinding);

        VkDescriptorSetLayoutBinding cameraPosBinding{};
        cameraPosBinding.binding = 4;
        cameraPosBinding.descriptorCount = 1;
        cameraPosBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraPosBinding.pImmutableSamplers = nullptr;
        cameraPosBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(cameraPosBinding);

        VkDescriptorSetLayoutCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        CreateInfo.pBindings = setLayoutBindings.data();
        CreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
        if (vkCreateDescriptorSetLayout(externDevice, &CreateInfo, nullptr, &m_descriptorSetLayout_uniforms) != VK_SUCCESS) throw ERR_SET_LAYOUT_CREATION;
        setLayoutBindings.clear();

        VkDescriptorSetLayoutBinding dTextureBinding{};
        dTextureBinding.binding = 2;
        dTextureBinding.descriptorCount = 1;
        dTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        dTextureBinding.pImmutableSamplers = nullptr;
        dTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(dTextureBinding);

        VkDescriptorSetLayoutBinding sTextureBinding{};
        sTextureBinding.binding = 3;
        sTextureBinding.descriptorCount = 1;
        sTextureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        sTextureBinding.pImmutableSamplers = nullptr;
        sTextureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(sTextureBinding);

        CreateInfo = {};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        CreateInfo.pBindings = setLayoutBindings.data();
        CreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
        if (vkCreateDescriptorSetLayout(externDevice, &CreateInfo, nullptr, &m_descriptorSetLayout_samplers) != VK_SUCCESS) throw ERR_SET_LAYOUT_CREATION;
    }

    if (programType == TEST_PIPELINE) {
        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;

        VkDescriptorSetLayoutBinding colorBinding{};
        colorBinding.binding = 0;
        colorBinding.descriptorCount = 1;
        colorBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        colorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(colorBinding);

        VkDescriptorSetLayoutBinding samplerTest{};
        samplerTest.binding = 1;
        samplerTest.descriptorCount = 1;
        samplerTest.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerTest.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        setLayoutBindings.push_back(samplerTest);

        VkDescriptorSetLayoutCreateInfo CreateInfo{};
        CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        CreateInfo.pBindings = setLayoutBindings.data();
        CreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
        if (vkCreateDescriptorSetLayout(externDevice, &CreateInfo, nullptr, &m_descriptorSetLayout_uniforms) != VK_SUCCESS) throw ERR_SET_LAYOUT_CREATION;
    }
}

void Program::CreateDescriptorPool(ProgramType programType) {
    if (programType == MAIN_PIPELINE) {

        std::vector<VkDescriptorPoolSize> poolSizes;
        VkDescriptorPoolSize mvp{};
        mvp.descriptorCount = 1000;
        mvp.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes.push_back(mvp);

        VkDescriptorPoolSize pointLightData{};
        pointLightData.descriptorCount = 1000;
        pointLightData.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes.push_back(pointLightData);

        VkDescriptorPoolSize cameraPos{};
        cameraPos.descriptorCount = 1000;
        cameraPos.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes.push_back(cameraPos);

        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.poolSizeCount = (uint32_t)poolSizes.size();
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = (uint32_t)10000;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        if (vkCreateDescriptorPool(externDevice, &createInfo, nullptr, &m_descriptorPool_uniforms) != VK_SUCCESS) throw ERR_DESCRIPTOR_POOL_CREATION;
        poolSizes.clear();

        VkDescriptorPoolSize dTexture{};
        dTexture.descriptorCount = 1000;
        dTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes.push_back(dTexture);

        VkDescriptorPoolSize sTexture{};
        sTexture.descriptorCount = 1000;
        sTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes.push_back(sTexture);

        createInfo = {};
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.poolSizeCount = (uint32_t)poolSizes.size();
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = (uint32_t)10000;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        if (vkCreateDescriptorPool(externDevice, &createInfo, nullptr, &m_descriptorPool_samplers) != VK_SUCCESS) throw ERR_DESCRIPTOR_POOL_CREATION;
    }

    if (programType == TEST_PIPELINE)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        VkDescriptorPoolSize color{};
        color.descriptorCount = 1000;
        color.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes.push_back(color);

        VkDescriptorPoolSize image{};
        image.descriptorCount = 1000;
        image.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes.push_back(image);


        VkDescriptorPoolCreateInfo createInfo{};
        createInfo.pPoolSizes = poolSizes.data();
        createInfo.poolSizeCount = (uint32_t)poolSizes.size();
        createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        createInfo.maxSets = (uint32_t)1000;
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        if (vkCreateDescriptorPool(externDevice, &createInfo, nullptr, &m_descriptorPool_uniforms) != VK_SUCCESS) throw ERR_DESCRIPTOR_POOL_CREATION;
    }

}

void Program::CreateRenderpass(VkFormat SwapchainFormat) {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = SwapchainFormat;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = externDepthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkAttachmentReference colorAttachmentReference{};
    colorAttachmentReference.attachment = (uint32_t)0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference{};
    depthAttachmentReference.attachment = (uint32_t)1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    /*VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = (uint32_t)1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

    VkAttachmentDescription attachments[] = {
        colorAttachment,depthAttachment
    };*/

    VkSubpassDescription subpassDescription{};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = (uint32_t)1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

    VkAttachmentDescription attachments[] = {
        colorAttachment, depthAttachment
    };

    VkRenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.pAttachments = attachments;
    renderPassCreateInfo.attachmentCount = (uint32_t)2;
    renderPassCreateInfo.pSubpasses = &subpassDescription;
    renderPassCreateInfo.subpassCount = (uint32_t)1;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    if (vkCreateRenderPass(externDevice, &renderPassCreateInfo, nullptr, &m_renderpass) != VK_SUCCESS) throw ERR_RENDERPASS_CREATION;

}

Program::Program(ProgramType programType, VkFormat SwapchainFormat, int width, int height){
    
    CreateDescriptorSetLayout(programType);
    CreateDescriptorPool(programType);

    CreateRenderpass(SwapchainFormat);
    std::vector<char> vertexShader;
    std::vector<char> fragmentShader;

    if (programType == MAIN_PIPELINE) {
        vertexShader = ReadShader("SPIRV/programForObj_vert.spv");
        fragmentShader = ReadShader("SPIRV/programForObj_frag.spv");
    }

    if (programType == TEST_PIPELINE) {
        vertexShader = ReadShader("SPIRV/programForObj_vert_test.spv");
        fragmentShader = ReadShader("SPIRV/programForObj_frag_test.spv");
    }

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

    VkVertexInputBindingDescription bindingDescription;
    bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(DataTypes::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkPipelineDepthStencilStateCreateInfo depthState{};
    depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthState.depthTestEnable = VK_TRUE;
    depthState.depthWriteEnable = VK_TRUE;
    depthState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthState.depthBoundsTestEnable = VK_FALSE;

    VkVertexInputAttributeDescription attribDescription{};
    attribDescription.binding = 0;
    attribDescription.location = 0;
    attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescription.offset = offsetof(DataTypes::Vertex, position);
    attributeDescriptions.push_back(attribDescription);

    attribDescription.binding = 0;
    attribDescription.location = 1;
    attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescription.offset = offsetof(DataTypes::Vertex, color);
    attributeDescriptions.push_back(attribDescription);

    attribDescription.binding = 0;
    attribDescription.location = 2;
    attribDescription.format = VK_FORMAT_R32G32_SFLOAT;
    attribDescription.offset = offsetof(DataTypes::Vertex, UV);
    attributeDescriptions.push_back(attribDescription);

    attribDescription.binding = 0;
    attribDescription.location = 3;
    attribDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribDescription.offset = offsetof(DataTypes::Vertex, normal);
    attributeDescriptions.push_back(attribDescription);
    

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexBindingDescriptionCount = (uint32_t)1;

    VkPipelineInputAssemblyStateCreateInfo assemblyInfo = {};
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkShaderModule vertexModule = CreateShaderModule(externDevice, vertexShader);
    VkShaderModule fragmentModule = CreateShaderModule(externDevice, fragmentShader);

    VkPipelineShaderStageCreateInfo vertexStage = {};
    vertexStage.module = vertexModule;
    vertexStage.sType = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vertexStage.pName = "main";                   
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo fragmentStage = {};
    fragmentStage.module = fragmentModule;
    fragmentStage.sType = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    fragmentStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentStage.pName = "main";                      

    VkPipelineShaderStageCreateInfo stages[] = { vertexStage,fragmentStage };

    VkViewport viewport = {};
    viewport.x = 0;
    viewport.y = height;
    viewport.height = -(float)height;
    viewport.width = (float)width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissors = {};
    scissors.extent.height = height;
    scissors.extent.width = width;
    scissors.offset = {};

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pScissors = &scissors;
    viewportStateCreateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
    rasterizationInfo.sType = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.depthBiasEnable = VK_FALSE;
    rasterizationInfo.cullMode = VK_CULL_MODE_NONE;//Не отрисовывать полигоны на заднем плане//
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;//Заполнение цветом треугольников//
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;//Соединение вершин против часовой стрелки//

    VkPipelineMultisampleStateCreateInfo multisampleInfo = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleInfo.minSampleShading = 0.2f;
    multisampleInfo.pSampleMask = nullptr;


    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlendInfo = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendInfo.pAttachments = &colorBlendAttachment;
    colorBlendInfo.attachmentCount = 1;

    VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    //схема передачи наборов дескрипторов (локации юниформ)//
    VkDescriptorSetLayout setLayouts[] = { m_descriptorSetLayout_uniforms, m_descriptorSetLayout_samplers};
    PipelineLayoutInfo.pSetLayouts = setLayouts;
    PipelineLayoutInfo.setLayoutCount = 2;


    //VkPushConstantRange pushConstantRange{};
    ////свойства констант, которые можно передать без буферов//
    //pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    //pushConstantRange.size = sizeof(DataTypes::PushConstants);//Размер структуры с константами//
    //pushConstantRange.offset = 0;

    //PipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
    //PipelineLayoutInfo.pushConstantRangeCount = uint32_t(1);

    if (vkCreatePipelineLayout(externDevice, &PipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        throw ERR_PIPELINE_LAYOUT_CREATION;
    

    m_pipelineCreateInfo.sType = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    

   /* if (ENABLE_DYNAMIC_VIEWPORT) {
        VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateInfo = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
        dynamicStateInfo.dynamicStateCount = 2;
        dynamicStateInfo.pDynamicStates = dynamicStates;

        CreateInfo.pDynamicState = &dynamicStateInfo;
    }*/

    m_pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    m_pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    m_pipelineCreateInfo.pMultisampleState = &multisampleInfo;
    m_pipelineCreateInfo.pRasterizationState = &rasterizationInfo;
    m_pipelineCreateInfo.renderPass = m_renderpass;
    m_pipelineCreateInfo.stageCount = 2;
    m_pipelineCreateInfo.pStages = stages;
    m_pipelineCreateInfo.pInputAssemblyState = &assemblyInfo;
    m_pipelineCreateInfo.subpass = 0;
    m_pipelineCreateInfo.layout = m_pipelineLayout;
    m_pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    m_pipelineCreateInfo.pDepthStencilState = &depthState;

    if (vkCreateGraphicsPipelines(externDevice, 0, 1, &m_pipelineCreateInfo, nullptr, &m_pipeline) != VK_SUCCESS) throw ERR_PIPELINE_CREATION;

    vkDestroyShaderModule(externDevice, vertexModule, nullptr);
    vkDestroyShaderModule(externDevice, fragmentModule, nullptr);
}


void Program::UseProgram(){}
Program::~Program(){

    vkDestroyDescriptorPool(externDevice, m_descriptorPool_samplers, nullptr);
    vkDestroyDescriptorPool(externDevice, m_descriptorPool_uniforms, nullptr);
    
    vkDestroyDescriptorSetLayout(externDevice, m_descriptorSetLayout_samplers, nullptr);
    vkDestroyDescriptorSetLayout(externDevice, m_descriptorSetLayout_uniforms, nullptr);

    vkDestroyPipelineLayout(externDevice, m_pipelineLayout, nullptr);
    vkDestroyRenderPass(externDevice, m_renderpass, nullptr);
    vkDestroyPipeline(externDevice, m_pipeline, nullptr);
}


#endif