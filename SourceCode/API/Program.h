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
    MAIN_PIPELINE,
    TEST_PIPELINE
};

class Program {
    #ifdef USE_GL
        GLuint m_program;
    #endif

    #ifdef USE_VK
        VkDescriptorSetLayout m_descriptorSetLayout_uniforms;
        VkDescriptorPool m_descriptorPool_uniforms;

        VkDescriptorSetLayout m_descriptorSetLayout_samplers;
        VkDescriptorPool m_descriptorPool_samplers;
    
        VkPipelineLayout m_pipelineLayout;
        VkPipeline m_pipeline;
        VkGraphicsPipelineCreateInfo m_pipelineCreateInfo{};
        VkRenderPass m_renderpass;

        void CreateRenderpass(VkFormat SwapchainFormat);

        VkShaderModule CreateShaderModule(VkDevice device, std::vector<char>& code);
        void CreateDescriptorSetLayout(ProgramType programType);
        void CreateDescriptorPool(ProgramType programType);
    #endif

    public:
 
    #ifdef USE_VK
        VkPipeline GetPipeline();
        VkRenderPass GetRenderPass() {
            return m_renderpass;
        }
        VkPipelineLayout GetPipelineLayout() {
            return m_pipelineLayout;
        }

        VkDescriptorSetLayout GetSetLayout_Uniforms() {
            return m_descriptorSetLayout_uniforms;
        }

        VkDescriptorPool GetDescriptorPool_Uniforms() {
            return m_descriptorPool_uniforms;
        }

        VkDescriptorSetLayout GetSetLayout_Samplers() {
            return m_descriptorSetLayout_samplers;
        }

        VkDescriptorPool GetDescriptorPool_Samplers() {
            return m_descriptorPool_samplers;
        }

        Program(ProgramType programType, VkFormat SwapchainFormat, int width, int height);

    #endif
    public:

    #ifdef USE_GL
        Program(ProgramType programType);
        void UniformMatrix4fv(std::string name, glm::mat4& data);
        void Uniform3f(std::string name, glm::vec3& data);
        void Uniform4f(std::string name, glm::vec4& data);
        void UniformPointLightData(DataTypes::PointLightData data, int arrayIndex);
        void Uniform1i(std::string name, int data);
        void Uniform1f(std::string name, float data);
    #endif

    
    




    void UseProgram();
    ~Program();
};