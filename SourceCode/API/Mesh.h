#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <map>

using namespace DataTypes;

enum ShapeType {
    Triangle,
    Rectangle,
    Cube
};

class Mesh {
    std::vector<int> material_ID;
    //std::map<int, std::vector<Texture*>> dTextures;

    std::map<int, Texture*> dTextures;
    std::map<int, Texture*> sTextures;

    std::vector<VertexArray*> vertexArrays;

    void LoadMaterials(const aiScene* scene, std::string path);
public:

    Mesh(ShapeType shapeType, std::string TexturePath);
    Mesh(ShapeType shapeType, glm::vec3 color);
    Mesh(std::string path);
    Mesh(std::string path, glm::vec3 color);
    void Draw();
    ~Mesh();

};