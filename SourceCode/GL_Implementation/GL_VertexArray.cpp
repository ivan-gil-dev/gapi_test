#include "../API/VertexArray.h"

#ifdef USE_GL

VertexArray::VertexArray(std::vector<DataTypes::Vertex> vertices, std::vector<unsigned int> indices) {
    m_vertices = vertices;
    m_indices = indices;

   
    glGenVertexArrays(1, &m_vertexArray);//Выделить память для массива вертексов
    glBindVertexArray(m_vertexArray);    //Выбор текущего используемого массива вертексов


    glGenBuffers(1, &m_vertexBuffer);                //Выделить память для буфера, хранящего параметры вершин
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);   //Выбор текущего используемого буфера, хранящего параметры вершин
    glBufferData(                              //Запись данных в буфер
        GL_ARRAY_BUFFER,                       //Тип буфера: массив данных
        sizeof(vertices[0]) * vertices.size(), //Размер передаваемого массива
        vertices.data(),                       //Указатель на первый элемент массива параметров
        GL_STATIC_DRAW                         //Назначение массива
    );

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, //Тип буфера: массив индексов
        sizeof(indices[0]) * indices.size(),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer( //Указатель на параметр вертекса (координаты вершины)
        0, // location = 0 в вершинном шейдере
        3, // считать три элемента
        GL_FLOAT, // тип данных
        GL_FALSE, // нормирование
        sizeof(DataTypes::Vertex), // шаг считывания данных из массива triangleData 
        (void*)0 // сдвиг относительно шага считывания данных
    );

    glEnableVertexAttribArray(0); //Включить передачу данных для location = 0

    glVertexAttribPointer( //Указатель на параметр вертекса (цвет вершины)
        1, // location = 1 в вершинном шейдере
        3, // считать три элемента
        GL_FLOAT, // тип данных
        GL_FALSE, // нормирование
        sizeof(DataTypes::Vertex), // шаг считывания данных из массива triangleData 
        (void*)(3 * sizeof(float)) // сдвиг относительно шага считывания данных
    );

    glEnableVertexAttribArray(1); //Включить передачу данных для location = 1

    glVertexAttribPointer( //Указатель на параметр вертекса (цвет вершины)
        2, // location = 2 в вершинном шейдере
        2, // считать два элемента
        GL_FLOAT, // тип данных
        GL_FALSE, // нормирование
        sizeof(DataTypes::Vertex), // шаг считывания данных из массива triangleData 
        (void*)(6 * sizeof(float)) // сдвиг относительно шага считывания данных
    );

    glEnableVertexAttribArray(2); //Включить передачу данных для location = 2


    glVertexAttribPointer( //Указатель на параметр вертекса (цвет вершины)
        3, // location = 3 в вершинном шейдере
        3, // считать два элемента
        GL_FLOAT, // тип данных
        GL_FALSE, // нормирование
        sizeof(DataTypes::Vertex), // шаг считывания данных из массива triangleData 
        (void*)(8 * sizeof(float)) // сдвиг относительно шага считывания данных
    );

    glEnableVertexAttribArray(3); //Включить передачу данных для location = 3


}

GLuint VertexArray::GetVertexBuffer() {
    return m_vertexBuffer;
}
GLuint VertexArray::GetIndexBuffer() {
    return m_indexBuffer;
}

GLuint VertexArray::GetVertexArray() {
    return m_vertexArray;
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_vertexArray);
    glDeleteBuffers(1, &m_vertexBuffer);
    glDeleteBuffers(1, &m_indexBuffer);
}

std::vector<unsigned int> VertexArray::GetIndices() {
    return m_indices;
}

#endif