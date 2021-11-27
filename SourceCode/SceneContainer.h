#pragma once
#include "Object.h"

class SceneContainer {
    std::vector<Object*> m_objects;
    std::vector<PointLight*> m_pointLights;

    //std::vector<glm::vec3*> m_pointLight_pos_arr;
    std::vector<PointLightData*> m_pointLightData_arr;
    public:
    void AddDefaultObject(Object* obj) {
        m_objects.push_back(obj);
    }

    void AddPointLightObject(PointLight* pointLight) {
        m_objects.push_back(pointLight);
        m_pointLights.push_back(pointLight);
        m_pointLightData_arr.push_back(pointLight->GetPointLightData());
        //m_pointLight_pos_arr.push_back(&pointLight->p_m_transformMatrices->pos);
    }

    std::vector<Object*>* GetObjects() {
        return &m_objects;
    }

    std::vector<PointLight*>* GetPointLights() {
        return &m_pointLights;
    }

    ~SceneContainer(){
        for (size_t i = 0; i < m_objects.size(); i++) delete m_objects[i];
    }

};