#pragma once

#include "DataTypes.h"
#include "API/Mesh.h"

using namespace DataTypes;
enum ObjectType {
    OBJECT_TYPE_DEFAULT,
    OBJECT_TYPE_POINT_LIGHT
};


class Object {
protected:
    ObjectType m_type;
public:
    TransformMatrices* p_m_transformMatrices;
    Mesh* p_m_mesh;
public:

    #ifdef USE_GL
        void Draw() {
            if (p_m_mesh) p_m_mesh->Draw();
        }
    #endif

    #ifdef USE_VK
        void Draw(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout Layout, int imageIndex) {
            if (p_m_mesh) p_m_mesh->Draw(commandBuffer, pipeline, Layout, imageIndex);
        }
    #endif



    Object() {
        m_type = OBJECT_TYPE_DEFAULT;
        p_m_transformMatrices = new TransformMatrices;
        p_m_mesh = nullptr;
    }

    ~Object() {
        if (p_m_mesh) delete p_m_mesh;
        delete p_m_transformMatrices;
    }

    virtual void Update(double DeltaTime) {};
};

class PointLight : public Object{
    protected:
    PointLightData *p_m_data;
    public:

    PointLight() {
        m_type = OBJECT_TYPE_POINT_LIGHT;
        p_m_data = new PointLightData;
        p_m_data->color = glm::vec3(1.f,1.0f,1.f);
        p_m_data->phongParams.w = true;
    }

    PointLightData* GetPointLightData() {
        p_m_data->pos = p_m_transformMatrices->pos;
        return p_m_data;
    }

    ~PointLight() {
        delete p_m_data;
    }
};

