#include "../CoreClass.h"

#ifdef USE_GL


void windowResizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

CoreClass::CoreClass(WindowProperties properties) : m_properties(properties) {
    if (!glfwInit()) throw ERR_GLFW_INIT_FAIL;
    p_m_window = glfwCreateWindow(m_properties.width, m_properties.height, m_properties.title, nullptr, nullptr);
    glfwMakeContextCurrent(p_m_window);

    m_clearColor = glm::vec3(0.2,0.2,0.2);

    glfwSetWindowSizeCallback(p_m_window, windowResizeCallback);
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw ERR_GLAD_INIT_FAIL;
    p_m_program = new Program;

    glEnable(GL_DEPTH_TEST);

}

CoreClass::~CoreClass() {
    delete m_sceneContainer;
    delete p_m_program;
    glfwDestroyWindow(p_m_window);
    glfwTerminate();
}



void CoreClass::UpdateUniformsForObject(int i) {
    glm::vec3 nullVector = glm::vec3(0.0f);
    //Вычислить произведения матрицы переноса, вращения и масштабирования
    DataTypes::MVP mvp;
    mvp.model = m_sceneContainer->GetObjects()->at(i)->p_m_transformMatrices->GetModelMatrix();
    mvp.view = m_camera.GetViewMatrix();
    mvp.projection = m_camera.GetProjectionMatrix();

    p_m_program->UniformMatrix4fv("model", mvp.model);
    p_m_program->UniformMatrix4fv("view", mvp.view);
    p_m_program->UniformMatrix4fv("projection", mvp.projection);

    glm::vec3 cameraPos = m_camera.GetPos();
    p_m_program->Uniform3f("cameraPos", cameraPos);

    for (size_t i = 0; i < MAX_POINTLIGHT_COUNT; i++)
    {
        if (i < m_sceneContainer->GetPointLights()->size()) {
            DataTypes::PointLightData data = *m_sceneContainer->GetPointLights()->at(i)->GetPointLightData();
            p_m_program->UniformPointLightData(data,i);
        }
        else {
            DataTypes::PointLightData data{};
            p_m_program->UniformPointLightData(data, i);
        }
    }
}

void CoreClass::Play(SceneContainer* sceneContainer) {

    m_sceneContainer = sceneContainer;    
    p_m_program->UseProgram();
    p_m_program->Uniform1i("dTexture", 0);
    p_m_program->Uniform1i("sTexture", 1);

    while (!glfwWindowShouldClose(p_m_window))//Пока окно не закрыто
    {
        auto beginTime = Timer.now();//Получаем время начала итерации цикла

        //Цвет обновления окна
        glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, 1.0f);

        //Стереть все с экрана перед отрисовкой следующего кадра
        //(Освободить буфер, хранящий цвет пикселей примитивов и освободить буфер глубины)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Выставить значение состояния текущей используемой программы
        p_m_program->UseProgram();
        m_camera.Update(p_m_window, DeltaTime);

        for (size_t i = 0; i < m_sceneContainer->GetObjects()->size(); i++)
        {

            m_sceneContainer->GetObjects()->at(i)->Update(DeltaTime);
            UpdateUniformsForObject(i);
            m_sceneContainer->GetObjects()->at(i)->Draw();
        }

        //Обработать оконные события
        glfwPollEvents();

        //Переключить буфер, из которого необходимо вывести изображение
        glfwSwapBuffers(p_m_window);

        auto endTime = Timer.now();//Получаем время конца итерации цикла

        //Вычисляем разность между временем начала и конца итерации цикла
        DeltaTime = (double)std::chrono::duration_cast<std::chrono::microseconds>(endTime - beginTime).count();
        //Перевод в секунды
        DeltaTime /= 1000000;
    }
}

#endif