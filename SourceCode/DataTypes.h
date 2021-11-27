#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#define MAX_POINTLIGHT_COUNT 32
#define USE_GL
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
        
        bool isUsed;
        glm::vec3 pos;
        glm::vec3 color;

        float ambientMultiplier;
        float diffuseMultiplier;
        float specularMultiplier;

        PointLightData() {
            ambientMultiplier = 0.1f;
            diffuseMultiplier = 0.5f;
            specularMultiplier = 0.9f;
            color = glm::vec3(0,0,0);
            pos = glm::vec3(0,0,0);
            isUsed = false;
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
