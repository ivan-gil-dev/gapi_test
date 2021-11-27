#include "../API/VertexArray.h"

#ifdef USE_GL

VertexArray::VertexArray(std::vector<DataTypes::Vertex> vertices, std::vector<unsigned int> indices) {
    m_vertices = vertices;
    m_indices = indices;

   
    glGenVertexArrays(1, &m_vertexArray);//�������� ������ ��� ������� ���������
    glBindVertexArray(m_vertexArray);    //����� �������� ������������� ������� ���������


    glGenBuffers(1, &m_vertexBuffer);                //�������� ������ ��� ������, ��������� ��������� ������
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);   //����� �������� ������������� ������, ��������� ��������� ������
    glBufferData(                              //������ ������ � �����
        GL_ARRAY_BUFFER,                       //��� ������: ������ ������
        sizeof(vertices[0]) * vertices.size(), //������ ������������� �������
        vertices.data(),                       //��������� �� ������ ������� ������� ����������
        GL_STATIC_DRAW                         //���������� �������
    );

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, //��� ������: ������ ��������
        sizeof(indices[0]) * indices.size(),
        indices.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer( //��������� �� �������� �������� (���������� �������)
        0, // location = 0 � ��������� �������
        3, // ������� ��� ��������
        GL_FLOAT, // ��� ������
        GL_FALSE, // ������������
        sizeof(DataTypes::Vertex), // ��� ���������� ������ �� ������� triangleData 
        (void*)0 // ����� ������������ ���� ���������� ������
    );

    glEnableVertexAttribArray(0); //�������� �������� ������ ��� location = 0

    glVertexAttribPointer( //��������� �� �������� �������� (���� �������)
        1, // location = 1 � ��������� �������
        3, // ������� ��� ��������
        GL_FLOAT, // ��� ������
        GL_FALSE, // ������������
        sizeof(DataTypes::Vertex), // ��� ���������� ������ �� ������� triangleData 
        (void*)(3 * sizeof(float)) // ����� ������������ ���� ���������� ������
    );

    glEnableVertexAttribArray(1); //�������� �������� ������ ��� location = 1

    glVertexAttribPointer( //��������� �� �������� �������� (���� �������)
        2, // location = 2 � ��������� �������
        2, // ������� ��� ��������
        GL_FLOAT, // ��� ������
        GL_FALSE, // ������������
        sizeof(DataTypes::Vertex), // ��� ���������� ������ �� ������� triangleData 
        (void*)(6 * sizeof(float)) // ����� ������������ ���� ���������� ������
    );

    glEnableVertexAttribArray(2); //�������� �������� ������ ��� location = 2


    glVertexAttribPointer( //��������� �� �������� �������� (���� �������)
        3, // location = 3 � ��������� �������
        3, // ������� ��� ��������
        GL_FLOAT, // ��� ������
        GL_FALSE, // ������������
        sizeof(DataTypes::Vertex), // ��� ���������� ������ �� ������� triangleData 
        (void*)(8 * sizeof(float)) // ����� ������������ ���� ���������� ������
    );

    glEnableVertexAttribArray(3); //�������� �������� ������ ��� location = 3


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