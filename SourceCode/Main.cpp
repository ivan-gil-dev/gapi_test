#include "CoreClass.h"
#include "ErrorCodes.h"
#include <iostream>
#include <time.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

void ShowErrorMessage(int e);

class MovingLight : public PointLight{

    public:
    float speed = 1.0f;

    virtual void Update(double DeltaTime) override {
        p_m_transformMatrices->Translate(
            speed * float(DeltaTime) * glm::normalize(glm::cross(p_m_transformMatrices->pos,glm::vec3(0,1,0)))
        );

    }
   
};


int main() {
    WindowProperties properties;
    properties.title = "App";
    properties.width = 1366;
    properties.height = 768; 

    srand(time(NULL));
    

    try{
        CoreClass app(properties);

        SceneContainer *container = new SceneContainer;     


        /*for (size_t i = 1; i < 9; i++)
        {
            Object* obj = new Object;
            obj->p_m_mesh = new Mesh("assets\\teapot.fbx",glm::vec3(255/i,125,125));
            obj->p_m_transformMatrices->Translate(glm::vec3(10.0f, 7.0f, 10.0f));
            obj->p_m_transformMatrices->Scale({3,3,3});
            for (size_t j = 0; j < i; j++)
            {
                obj->p_m_transformMatrices->Translate(
                    glm::vec3(9) * normalize(glm::cross(obj->p_m_transformMatrices->pos, glm::vec3(0, 1, 0)))
                );
            }
            
            container->AddDefaultObject(obj);
        }*/
        
        Object* obj = new Object;
        obj->p_m_mesh = new Mesh("assets\\sponza\\sponza.obj");
        container->AddDefaultObject(obj);
        

        //Object* obj = new Object;
        //obj->p_m_mesh = new Mesh(ShapeType::Rectangle,"assets\\sponza\\textures\\vase_plant.png");
        //obj->p_m_transformMatrices->Scale(glm::vec3(5,5,5));
        //container->AddDefaultObject(obj);

        PointLight*
            pLight = new PointLight;
        pLight->p_m_transformMatrices->Translate(glm::vec3(0, 100.f, 0));
        pLight->p_m_mesh = new Mesh(ShapeType::Cube, glm::vec3(255, 255, 255));
        pLight->GetPointLightData()->ambientMultiplier = 0.2;
        pLight->GetPointLightData()->diffuseMultiplier = 500.0;
        pLight->GetPointLightData()->specularMultiplier = 0.5;


        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        pLight->GetPointLightData()->color = glm::vec3(
            r, g, b
        );

        container->AddPointLightObject(pLight);


        for (size_t i = 2; i < MAX_POINTLIGHT_COUNT + 1; i++)
        {
            MovingLight*
            pLight = new MovingLight;
            pLight->speed = 30;
            pLight->p_m_transformMatrices->Translate(glm::vec3(i*7, 2.f, i*7));
            pLight->p_m_mesh = new Mesh(ShapeType::Cube, glm::vec3(255, 255, 255));
            pLight->GetPointLightData()->ambientMultiplier = 0.2;
            pLight->GetPointLightData()->diffuseMultiplier = 5.0;
            pLight->GetPointLightData()->specularMultiplier = 0.5;


            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float g = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float b = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            pLight->GetPointLightData()->color = glm::vec3(
                r, g, b
            );

            container->AddPointLightObject(pLight);
        }

        


        
        app.Play(container);
    }
    catch (const int& e){
        ShowErrorMessage(e);
        return e;
    }

    return 0;
}

void ShowErrorMessage(int e) {
    if (e == ERR_GLFW_INIT_FAIL) std::cout << "Failed to init GLFW" << std::endl;
    if (e == ERR_GLAD_INIT_FAIL) std::cout << "Failed to init GLAD" << std::endl;
}