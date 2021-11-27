
#include "../API/Mesh.h"
#ifdef USE_GL
void Mesh::LoadMaterials(const aiScene* scene, std::string path) {
for (size_t i = 0; i < material_ID.size(); i++) {

    aiMaterial* mat = scene->mMaterials[material_ID[i]];
    aiColor3D diffuseColor(0.f, 0.f, 0.f);

    if (!dTextures.count(material_ID[i])) {
        if (scene->mMaterials[material_ID[i]]->GetTextureCount(aiTextureType_DIFFUSE))
        {
            for (size_t j = 0; j < scene->mMaterials[material_ID[i]]->GetTextureCount(aiTextureType_DIFFUSE); j++) {

                aiString str;
                scene->mMaterials[material_ID[i]]->GetTexture(aiTextureType_DIFFUSE, j, &str);

                if (std::string(str.C_Str()).size()) {
                    std::string relativeToModelPath(str.C_Str());
                    std::string pathToModelFolder = path.substr(0, path.find_last_of("\\"));
                    std::string relativeToExecutablePath(pathToModelFolder + "\\" + relativeToModelPath);

                    for (size_t i = 0; i < relativeToExecutablePath.size(); i++)
                        if (relativeToExecutablePath[i] == '\\')
                            relativeToExecutablePath[i] = '/';


                    std::cout << relativeToExecutablePath << std::endl;
                    Texture* diffuseTexture = new Texture(relativeToExecutablePath);
                    dTextures[material_ID[i]]=(diffuseTexture);
                }
                else {
                    Texture* diffuseTexture = new Texture(glm::vec3{ 255,0,255 });
                    dTextures[material_ID[i]] = (diffuseTexture);
                }
            }
        }
        else if (aiReturn_SUCCESS == scene->mMaterials[material_ID[i]]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor))
        {
            scene->mMaterials[material_ID[i]]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            Texture* diffuseTexture = new Texture(glm::vec3{ diffuseColor.r * 255, diffuseColor.g * 255, diffuseColor.b * 255 });
            dTextures[material_ID[i]] = (diffuseTexture);
        }
        else {
            Texture* diffuseTexture = new Texture(glm::vec3{ 255, 255, 255 });
            dTextures[material_ID[i]] = (diffuseTexture);
        }
    }

    if (!sTextures.count(material_ID[i])) {
        if (scene->mMaterials[material_ID[i]]->GetTextureCount(aiTextureType_SPECULAR))
        {
            for (size_t j = 0; j < scene->mMaterials[material_ID[i]]->GetTextureCount(aiTextureType_SPECULAR); j++) {

                aiString str;
                scene->mMaterials[material_ID[i]]->GetTexture(aiTextureType_SPECULAR, j, &str);

                if (std::string(str.C_Str()).size()) {
                    std::string relativeToModelPath(str.C_Str());
                    std::string pathToModelFolder = path.substr(0, path.find_last_of("\\"));
                    std::string relativeToExecutablePath(pathToModelFolder + "\\" + relativeToModelPath);

                    for (size_t i = 0; i < relativeToExecutablePath.size(); i++)
                        if (relativeToExecutablePath[i] == '\\')
                            relativeToExecutablePath[i] = '/';


                    std::cout << relativeToExecutablePath << std::endl;
                    Texture* specularTexture = new Texture(relativeToExecutablePath);
                    sTextures[material_ID[i]] = (specularTexture);
                }
                else {
                    Texture* specularTexture = new Texture(glm::vec3{ 255,0,255 });
                    sTextures[material_ID[i]] = (specularTexture);
                }
            }
        }
        else if (aiReturn_SUCCESS == scene->mMaterials[material_ID[i]]->Get(AI_MATKEY_COLOR_SPECULAR, diffuseColor))
        {
            scene->mMaterials[material_ID[i]]->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            Texture* specularTexture = new Texture(glm::vec3{ diffuseColor.r * 255, diffuseColor.g * 255, diffuseColor.b * 255 });
            sTextures[material_ID[i]] = (specularTexture);
        }
        else {
            Texture* specularTexture = new Texture(glm::vec3{ 255, 255, 255 });
            sTextures[material_ID[i]] = (specularTexture);
        }
    }
}
}

Mesh::Mesh(ShapeType shapeType, std::string TexturePath) {
    std::vector<DataTypes::Vertex> vertices;
    std::vector<GLuint> indices;

    if (shapeType == ShapeType::Rectangle) {

        vertices.push_back(Vertex({ 0.5f,0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 1,1 }));
        vertices.push_back(Vertex({ 0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 1,0 }));
        vertices.push_back(Vertex({ -0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,0 }));
        vertices.push_back(Vertex({ -0.5f,0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,1 }));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);

    }
    if (shapeType == ShapeType::Triangle) {
        vertices.push_back(Vertex({ -0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,0 }));
        vertices.push_back(Vertex({ 0.f,0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,1 }));
        vertices.push_back(Vertex({ 0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 1,0 }));

        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(0);
    }
    if (shapeType == ShapeType::Cube) {

        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));

        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));

        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));

        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));

        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));

        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,   0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }));

        for (size_t i = 0; i < vertices.size(); i++) indices.push_back(i);
    }

    VertexArray* vArray = new VertexArray(vertices, indices);
    vertexArrays.push_back(vArray);

    material_ID.push_back(0);

    Texture* diffuseTexture = new Texture(TexturePath);
    dTextures[0] = (diffuseTexture);

    Texture* specularTexture = new Texture(glm::vec3(255, 255, 255));
    sTextures[0] = (specularTexture);

}

Mesh::Mesh(ShapeType shapeType, glm::vec3 color) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    if (shapeType == ShapeType::Rectangle) {

        vertices.push_back(Vertex({ 0.5f,0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 1,1 },  { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 1,0 }, { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,0 },{ 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f,0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,1 }, { 0,0,1 }));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);

    }
    if (shapeType == ShapeType::Triangle) {
        vertices.push_back(Vertex({ -0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f }, { 0,0 }, { 0,0,1 }));
        vertices.push_back(Vertex({ 0.f,0.5f,0.0f }, { 1.0f,1.0f,1.0f },    { 0,1 }, { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,-0.5f,0.0f }, { 1.0f,1.0f,1.0f },  { 1,0 }, { 0,0,1 }));

        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(0);
    }
    if (shapeType == ShapeType::Cube) {
        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },  { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },  { 0,0,-1 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,-1 }));

        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f },  { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },   { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },   { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },   { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },  { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f },  { 0,0,1 }));

        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },  { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },  { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },  { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f },  { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { -1,0,0 }));

        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },   { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },   { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },   { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },   { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f },   { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },   { 1,0,0 }));

        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },  { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { 0,-1,0 }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,-1,0 }));

        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },  { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f },  { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f },  { 0,1,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f },  { 0,1,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f },  { 0,1,0 }));

        for (size_t i = 0; i < vertices.size(); i++) indices.push_back(i);
    }

    VertexArray* vArray = new VertexArray(vertices, indices);

    vertexArrays.push_back(vArray);

    material_ID.push_back(0);

    Texture* diffuseTexture = new Texture(color);
    dTextures[0] = (diffuseTexture);

    Texture* specularTexture = new Texture(glm::vec3(1,1,1));
    sTextures[0] = (specularTexture);
}

Mesh::Mesh(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;

    for (size_t i = 0; i < scene->mNumMeshes; i++) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        for (size_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {

            Vertex v;

            v.position.x = scene->mMeshes[i]->mVertices[j].x;
            v.position.y = scene->mMeshes[i]->mVertices[j].y;
            v.position.z = scene->mMeshes[i]->mVertices[j].z;

            v.normal.x = scene->mMeshes[i]->mNormals[j].x;
            v.normal.y = scene->mMeshes[i]->mNormals[j].y;
            v.normal.z = scene->mMeshes[i]->mNormals[j].z;

            if (scene->mMeshes[i]->mColors[0]) {
                v.color.r = scene->mMeshes[i]->mColors[0][j].r;
                v.color.g = scene->mMeshes[i]->mColors[0][j].g;
                v.color.b = scene->mMeshes[i]->mColors[0][j].b;
            }

            if (scene->mMeshes[i]->mTextureCoords[0]) {
                v.UV.x = scene->mMeshes[i]->mTextureCoords[0][j].x;
                v.UV.y = scene->mMeshes[i]->mTextureCoords[0][j].y;
            }

            vertices.push_back(v);
        }


        for (size_t k = 0; k < scene->mMeshes[i]->mNumFaces; k++)
            for (size_t z = 0; z < scene->mMeshes[i]->mFaces[k].mNumIndices; z++)
                indices.push_back(scene->mMeshes[i]->mFaces[k].mIndices[z]);

        VertexArray* vertexArray = new VertexArray(vertices, indices);
        vertexArrays.push_back(vertexArray);

        material_ID.push_back(scene->mMeshes[i]->mMaterialIndex);
    }

    LoadMaterials(scene, path);


}

Mesh::Mesh(std::string path, glm::vec3 color) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;

    for (size_t i = 0; i < scene->mNumMeshes; i++) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        for (size_t j = 0; j < scene->mMeshes[i]->mNumVertices; j++) {

            Vertex v;

            v.position.x = scene->mMeshes[i]->mVertices[j].x;
            v.position.y = scene->mMeshes[i]->mVertices[j].y;
            v.position.z = scene->mMeshes[i]->mVertices[j].z;

            if (scene->mMeshes[i]->mColors[0]) {
                v.color.r = scene->mMeshes[i]->mColors[0][j].r;
                v.color.g = scene->mMeshes[i]->mColors[0][j].g;
                v.color.b = scene->mMeshes[i]->mColors[0][j].b;
            }

            if (scene->mMeshes[i]->mTextureCoords[0]) {
                v.UV.x = scene->mMeshes[i]->mTextureCoords[0][j].x;
                v.UV.y = scene->mMeshes[i]->mTextureCoords[0][j].y;
            }

            vertices.push_back(v);
        }


        for (size_t k = 0; k < scene->mMeshes[i]->mNumFaces; k++)
            for (size_t z = 0; z < scene->mMeshes[i]->mFaces[k].mNumIndices; z++)
                indices.push_back(scene->mMeshes[i]->mFaces[k].mIndices[z]);

        VertexArray* vertexArray = new VertexArray(vertices, indices);
        vertexArrays.push_back(vertexArray);

        material_ID.push_back(scene->mMeshes[i]->mMaterialIndex);

        Texture* diffuseTexture = new Texture(color);
        dTextures[scene->mMeshes[i]->mMaterialIndex] = (diffuseTexture);

        Texture* specularTexture = new Texture(color);
        sTextures[scene->mMeshes[i]->mMaterialIndex] = (specularTexture);
    }
}

void Mesh::Draw() {
    if (vertexArrays.size()) {
        for (size_t i = 0; i < vertexArrays.size(); i++)
        {

            dTextures[material_ID[i]]->BindTexture(GL_TEXTURE0);
            sTextures[material_ID[i]]->BindTexture(GL_TEXTURE1);

            //¬ыставить глобальное состо€ние текущего используемого массива вертексов
            glBindVertexArray(vertexArrays[i]->GetVertexArray());

            //¬ывести на экран примитив, хран€щийс€ в текущем 
            //используемом массиве вертексов с помощью индексов
            //(индексы тоже наход€тс€ в массиве вертексов)
            glDrawElements(
                GL_TRIANGLES,//“опологи€ соединени€ вершин
                vertexArrays[i]->GetIndices().size(),// оличество индексов (Ќе количество вершин!)
                GL_UNSIGNED_INT,//“ип данных индексов: беззнаковое целочисленное
                0
            );
        }
    }



}

Mesh::~Mesh() {

    std::map<int, Texture*>::iterator it;
    for (it = dTextures.begin(); it != dTextures.end(); it++)
    {
        delete it->second;
    }
    for (size_t i = 0; i < vertexArrays.size(); i++)
    {
        delete vertexArrays[i];
    }
}
#endif