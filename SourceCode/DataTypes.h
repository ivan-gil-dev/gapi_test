#pragma once
#define MAX_POINTLIGHT_COUNT 4
#define USE_VK
#define DEBUG
#define USE_FACE_CULLING


#ifdef USE_VK
    #define MAX_TEXTURE_SLOTS 32
    #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
    //#define DEBUG

    #include "ErrorCodes.h"
    #include <volk.h>
    
    #include <glm/glm.hpp>
    #include <glm/gtx/transform.hpp>
    
    struct VkQueueIndices {
        uint32_t queueIndex1_graphics;
        uint32_t queueIndex2_command;
    };
    namespace DataTypes {
        struct PushConstants {
            int material_ID;
        };
    }
    

#endif


#ifdef USE_GL
    #include <glm/glm.hpp>
    #include <glm/gtx/transform.hpp>
#endif

namespace DataTypes {
    struct Vertex {
        glm::vec3 position;
        glm::vec3 color;
        glm::vec2 UV;
        glm::vec3 normal;


        Vertex(glm::vec3 p, glm::vec3 c, glm::vec2 uv, glm::vec3 n) {
            position = p;
            color = c;
            UV = uv;
            normal = n;
        }

        Vertex(glm::vec3 p, glm::vec3 c, glm::vec2 uv) {
            position = p;
            color = c;
            UV = uv;
        }

        Vertex(glm::vec3 p, glm::vec3 c) {
            position = p;
            color = c;
            UV = glm::vec2(1.0f);
        }

        Vertex(glm::vec3 p) {
            position = p;
            color = glm::vec3(1.0f);
            UV = glm::vec2(1.0f);
        }

        Vertex() {
            position = glm::vec3(1.0f);
            color = glm::vec3(1.0f);
            UV = glm::vec2(1.0f);
        }


    };

    struct PointLightData {
        glm::vec3 pos;
        glm::vec3 color;

        glm::vec4 phongParams;
        
        //#ifdef USE_GL
        //        float ambientMultiplier;
        //        float diffuseMultiplier;
        //        float specularMultiplier;
        //        bool isUsed;
        //#endif // USE_GL
        //#ifdef USE_VK
        //        alignas(4) float ambientMultiplier;
        //        alignas(4) float diffuseMultiplier;
        //        alignas(4) float specularMultiplier;
        //        alignas(4) uint32_t isUsed;
        //#endif // USE_VK

        
        PointLightData() {
            phongParams.x = 0.1f;//Ambient
            phongParams.y = 0.5f;//Diffuse
            phongParams.z = 0.9f;//Specular
            phongParams.w = false;//Is Used

            color = glm::vec3(0,0,0);
            pos = glm::vec3(0,0,0);            
        }
    };

    struct MVP {
        glm::mat4  model, view, projection;
    };
    
    struct TransformMatrices {
        glm::mat4 translation, rotation, scale;
        glm::vec3 pos;

        void Translate(glm::vec3 v) {
            translation = glm::translate(
                translation,
                v
            );

            pos += v;
        }

        void Rotate(glm::vec3 a, float v) {
            rotation = glm::rotate(
                rotation,
                glm::radians(v),
                a
            );
        }

        void Scale(glm::vec3 v) {
            scale = glm::scale(
                scale,
                v
            );
        }

        TransformMatrices() {
            translation = glm::mat4(1.0f);
            rotation = glm::mat4(1.0f);
            scale = glm::mat4(1.0f);
            pos = glm::vec3(0.0f);
        }

        glm::mat4 GetModelMatrix() {
            return translation * rotation * scale;
        }
    };
}
