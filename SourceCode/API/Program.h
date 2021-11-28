#pragma once
#include "../DataTypes.h"
#ifdef USE_GL
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <vector>
enum ProgramType {
    MAIN_PIPELINE
};

class Program {
    #ifdef USE_GL
        GLuint m_program;
    #endif

    #ifdef USE_VK
        VkDescriptorSetLayout m_descriptorSetLayout;
        VkDescriptorPool m_descriptorPool;
        VkPipeline m_pipeline;
        VkGraphicsPipelineCreateInfo m_pipelineCreateInfo{};
        VkPipelineLayout m_pipelineLayout;
        VkRenderPass m_renderpass;
        void CreateRenderpass();
        VkShaderModule CreateShaderModule(VkDevice device, std::vector<char>& code);
        VkDescriptorSetLayout CreateDescriptorSetLayout(ProgramType programType);
    #endif

    public:
 
    #ifdef USE_VK
        VkRenderPass GetRenderPass() {
            return m_renderpass;
        }
    #endif


    public:
    Program(ProgramType programType);
 
    void UniformMatrix4fv(std::string name, glm::mat4& data);
    void Uniform3f(std::string name, glm::vec3& data);
    void UniformPointLightData(DataTypes::PointLightData data, int arrayIndex);

    void Uniform1i(std::string name, int data);
    void Uniform1f(std::string name, float data);

    void UseProgram();
    ~Program();
};