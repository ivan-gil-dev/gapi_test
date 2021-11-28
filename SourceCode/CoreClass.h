#pragma once
#include "ErrorCodes.h"
#include "API/Program.h"
#include "DataTypes.h"
#include "SceneContainer.h"
#include "Object.h"

#ifdef USE_GL
    #include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <iostream>
#include <chrono>
#ifdef USE_VK
    #include "Vulkan_Implementation/Vulkan_CommandPool.h"
#endif

struct WindowProperties {
    int width;
    int height;
    const char* title;
};


class Camera {
protected:
    glm::vec3  pos;
    glm::quat  orientation;
    glm::mat4  projection, translation;
    float      moveSpeed = 2.f;

    bool   firstMouse;
    float  last_x, last_y, yaw, pitch;

    virtual void MouseUpdate(GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            if (firstMouse) {
                last_x = xpos;
                last_y = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - last_x;
            float yoffset = last_y - ypos;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            last_x = xpos;
            last_y = ypos;

            yaw += xoffset;
            pitch += yoffset;

            if (pitch > 89) pitch = 89;
            if (pitch < -89) pitch = -89;

            glm::quat orientation1 = glm::angleAxis(glm::radians(-pitch), glm::vec3(1, 0, 0));
            glm::quat orientation2 = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
            orientation = orientation1 * orientation2;
            orientation = glm::normalize(orientation);

        }
        else {
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

public:
    Camera() {
        firstMouse = true;
        last_x = 0;
        last_y = 0;
        yaw = -90;
        pitch = 0;

        projection = glm::perspective(glm::radians(80.f), 16.f / 9, 0.1f, 10000.f);
        pos = glm::vec3(3.f, 3.f, 3.f);
        translation = glm::mat4(1.0f);
        translation = glm::translate(translation, -pos);


        glm::quat orientation1 = glm::angleAxis(glm::radians(-pitch), glm::vec3(1, 0, 0));
        glm::quat orientation2 = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
        orientation = orientation1 * orientation2;
        orientation = glm::normalize(orientation);

    }

    glm::mat4 GetProjectionMatrix() {
        return projection;
    }

    glm::mat4 GetViewMatrix() {
        return glm::mat4_cast(orientation) * translation;
    }

    glm::vec3 GetPos() {
        return pos;
    }

    virtual void Update(GLFWwindow* window, float deltaTime) {
        glm::quat inverseOrientation = glm::conjugate(orientation);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            pos += inverseOrientation * glm::vec3(0, 0, -1) * moveSpeed;
            translation = glm::mat4(1.0f);
            translation = glm::translate(translation, -pos);
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            pos -= inverseOrientation * glm::vec3(0, 0, -1) * moveSpeed;
            translation = glm::mat4(1.0f);
            translation = glm::translate(translation, -pos);
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            pos -= glm::cross(inverseOrientation * glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)) * moveSpeed;
            translation = glm::mat4(1.0f);
            translation = glm::translate(translation, -pos);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            pos += glm::cross(inverseOrientation * glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)) * moveSpeed;
            translation = glm::mat4(1.0f);
            translation = glm::translate(translation, -pos);
        }

        MouseUpdate(window);
    }

};


class CoreClass {
    GLFWwindow* p_m_window;    
    WindowProperties m_properties;
    Program *p_m_program;
    Camera m_camera;
    glm::vec3 m_clearColor;
    SceneContainer *m_sceneContainer;
    
    #ifdef USE_VK
        CommandPool *m_CommandPool;
        void PrepareFrame();
        void DrawFrame();
    #endif



    public:
    CoreClass(WindowProperties properties);
    ~CoreClass();
    //Инициализация таймера
    std::chrono::steady_clock Timer = std::chrono::high_resolution_clock();
    //Затраченное время для отрисовки одного кадра
    double DeltaTime = 0;
    void Play(SceneContainer* sceneContainer);
    void UpdateUniformsForObject(int i);

};