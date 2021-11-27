#pragma once
#include "../DataTypes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Program {
    #ifdef USE_GL
        GLuint m_program;
    #endif

    public:
    Program();
    void UniformMatrix4fv(std::string name, glm::mat4& data);
    void Uniform3f(std::string name, glm::vec3& data);
    void UniformPointLightData(DataTypes::PointLightData data, int arrayIndex);

    void Uniform1i(std::string name, int data);
    void Uniform1f(std::string name, float data);

    void UseProgram();
    ~Program();
};