#include "../API/Mesh.h"
#include "_Vulkan_Handles.h"
#ifdef USE_VK


void Mesh::InitDescriptorSets() {
    
    
    {
        std::vector<VkDescriptorSetLayout> setLayouts(extern_Swapchain_Image_View_Count, externSetLayout_uniforms);

        VkDescriptorSetAllocateInfo allocateInfo{};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = externDescriptorPool_uniforms;
        allocateInfo.descriptorSetCount = (uint32_t)setLayouts.size();
        allocateInfo.pSetLayouts = setLayouts.data();

        uniformDescriptorSets.resize(setLayouts.size());

        VkResult result = vkAllocateDescriptorSets(externDevice, &allocateInfo, uniformDescriptorSets.data());
        if (result != VK_SUCCESS) {
            std::cout << result << std::endl;
            throw std::runtime_error("Failed to allocate descriptor sets");
        }
    }

    
    for (size_t i = 0; i < uniformDescriptorSets.size(); i++)
    {
        std::vector<VkWriteDescriptorSet> writeDescriptorSets;

        VkWriteDescriptorSet mvpDescriptorSet{};
        mvpDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        mvpDescriptorSet.descriptorCount = 1;
        mvpDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        mvpDescriptorSet.dstSet = uniformDescriptorSets[i];
        mvpDescriptorSet.dstBinding = 0;
        mvpDescriptorSet.dstArrayElement = 0;
        mvpDescriptorSet.pBufferInfo = m_MVP_Uniform.GetDescriptor();
        writeDescriptorSets.push_back(mvpDescriptorSet);

        for (size_t k = 0; k < MAX_POINTLIGHT_COUNT; k++)
        {
            VkWriteDescriptorSet pointLightDataDescriptorSet{};
            pointLightDataDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            pointLightDataDescriptorSet.descriptorCount = 1;
            pointLightDataDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            pointLightDataDescriptorSet.dstSet = uniformDescriptorSets[i];
            pointLightDataDescriptorSet.dstBinding = 1;
            pointLightDataDescriptorSet.dstArrayElement = k;
            pointLightDataDescriptorSet.pBufferInfo = m_pointLightData_Uniform[k].GetDescriptor();
            writeDescriptorSets.push_back(pointLightDataDescriptorSet);
        }

        VkWriteDescriptorSet cameraPosDescriptorSet{};
        cameraPosDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        cameraPosDescriptorSet.descriptorCount = 1;
        cameraPosDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        cameraPosDescriptorSet.dstSet = uniformDescriptorSets[i];
        cameraPosDescriptorSet.dstBinding = 4;
        cameraPosDescriptorSet.dstArrayElement = 0;
        cameraPosDescriptorSet.pBufferInfo = m_CameraPos_Uniform.GetDescriptor();
        writeDescriptorSets.push_back(cameraPosDescriptorSet);

        vkUpdateDescriptorSets(externDevice, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
        writeDescriptorSets.resize(0);
    }

    std::vector<VkDescriptorSetLayout> setLayouts(extern_Swapchain_Image_View_Count, externSetLayout_samplers);

    for (size_t i = 0; i < material_ID.size(); i++){
        if (!materialDescriptorSets.count(material_ID[i]))
        {
            VkDescriptorSetAllocateInfo allocateInfo{};
            allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocateInfo.descriptorPool = externDescriptorPool_samplers;
            allocateInfo.descriptorSetCount = (uint32_t)setLayouts.size();
            allocateInfo.pSetLayouts = setLayouts.data();

            materialDescriptorSets[material_ID[i]].resize(setLayouts.size());

            VkResult result = vkAllocateDescriptorSets(externDevice, &allocateInfo, materialDescriptorSets[material_ID[i]].data());
            if (result != VK_SUCCESS) {
                std::cout << result << std::endl;
                throw std::runtime_error("Failed to allocate descriptor sets");
            }

            for (size_t j = 0; j < materialDescriptorSets[material_ID[i]].size(); j++) {

                std::vector<VkWriteDescriptorSet> writeDescriptorSets;

                VkWriteDescriptorSet dTextureDescriptorSet{};
                dTextureDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                dTextureDescriptorSet.descriptorCount = 1;
                dTextureDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                dTextureDescriptorSet.dstSet = materialDescriptorSets[material_ID[i]][j];
                dTextureDescriptorSet.dstBinding = 2;
                dTextureDescriptorSet.dstArrayElement = 0;
                dTextureDescriptorSet.pImageInfo = dTextures[material_ID[i]]->GetDescriptor();
                writeDescriptorSets.push_back(dTextureDescriptorSet);

                VkWriteDescriptorSet sTextureDescriptorSet{};
                sTextureDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                sTextureDescriptorSet.descriptorCount = 1;
                sTextureDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                sTextureDescriptorSet.dstSet = materialDescriptorSets[material_ID[i]][j];
                sTextureDescriptorSet.dstBinding = 3;
                sTextureDescriptorSet.dstArrayElement = 0;
                sTextureDescriptorSet.pImageInfo = sTextures[material_ID[i]]->GetDescriptor();
                writeDescriptorSets.push_back(sTextureDescriptorSet);

                vkUpdateDescriptorSets(externDevice, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
                writeDescriptorSets.resize(0);
            }
        }
    }

}

void Mesh::ClearDescriptorSets() {
    std::map<int, std::vector<VkDescriptorSet>>::iterator it;
    
    vkFreeDescriptorSets(
        externDevice,
        externDescriptorPool_uniforms,
        (uint32_t)uniformDescriptorSets.size(),
        uniformDescriptorSets.data()
    );
    
    for (it = materialDescriptorSets.begin(); it != materialDescriptorSets.end(); it++)
    {
        vkFreeDescriptorSets(
            externDevice,
            externDescriptorPool_samplers,
            (uint32_t)it->second.size(),
            it->second.data()
        );
    }
}

void Mesh::InitUniformBuffers() {
    m_MVP_Uniform.CreateUniformBuffer(externPhysicalDevice, externDevice, sizeof(DataTypes::MVP));
    m_CameraPos_Uniform.CreateUniformBuffer(externPhysicalDevice, externDevice, 3 * sizeof(float));
    for (size_t i = 0; i < MAX_POINTLIGHT_COUNT; i++){
        m_pointLightData_Uniform[i].CreateUniformBuffer(externPhysicalDevice, externDevice, sizeof(DataTypes::PointLightData));
    }
}

void Mesh::ClearUniformBuffers() {
    m_MVP_Uniform.Destroy(externDevice);
    m_CameraPos_Uniform.Destroy(externDevice);
    for (size_t i = 0; i < MAX_POINTLIGHT_COUNT; i++) {
        m_pointLightData_Uniform[i].Destroy(externDevice);
    }
}

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
                        dTextures[material_ID[i]] = (diffuseTexture);
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
    InitUniformBuffers();

    std::vector<DataTypes::Vertex> vertices;
    std::vector<unsigned int> indices;

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

    InitDescriptorSets();
}

Mesh::Mesh(ShapeType shapeType, glm::vec3 color) {
    InitUniformBuffers();
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (shapeType == ShapeType::Rectangle) {

        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.0f },{ 1.0f, 1.0f, 1.0f }, { 1,1 }, { 0,0,1 }));
        vertices.push_back(Vertex({  0.5f,-0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1,0 }, { 0,0,1 }));
        vertices.push_back(Vertex({  0.5f,0.5f, 0.0f },  { 1.0f, 1.0f, 1.0f }, { 0,0 }, { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f, 0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0,1 }, { 0,0,1 }));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        indices.push_back(2);
        indices.push_back(3);
        indices.push_back(0);

    }
    if (shapeType == ShapeType::Triangle) {
        vertices.push_back(Vertex({ -0.5f,-0.5f, 0.0f }, { 1.0f, 0.5f, 1.0f }, { 0,0 }, { 0,0,1 }));
        vertices.push_back(Vertex({  0.f,  0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 0,1 }, { 0,0,1 }));
        vertices.push_back(Vertex({  0.5f,-0.5f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1,0 }, { 0,0,1 }));

        indices.push_back(2);
        indices.push_back(1);
        indices.push_back(0);
    }
    if (shapeType == ShapeType::Cube) {
        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,0,-1 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, -0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,-1 }));

        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,0,1 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,0,1 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,0,1 }));

        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { -1,0,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { -1,0,0 }));

        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f, -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 1,0,0 }));
        vertices.push_back(Vertex({ 0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 1,0,0 }));

        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ 0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,-1,0 }));
        vertices.push_back(Vertex({ -0.5f,  -0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,-1,0 }));

        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,1.f }, { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,1,0 }));
        vertices.push_back(Vertex({ 0.5f,   0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 1.f,0.f }, { 0,1,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f, 0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,0.f }, { 0,1,0 }));
        vertices.push_back(Vertex({ -0.5f,  0.5f,-0.5f }, { 1.0f,1.0f,1.0f }, { 0.f,1.f }, { 0,1,0 }));

        for (size_t i = 0; i < vertices.size(); i++) indices.push_back(i);
    }

    VertexArray* vArray = new VertexArray(vertices, indices);

    vertexArrays.push_back(vArray);

    material_ID.push_back(0);

    Texture* diffuseTexture = new Texture(color);
    dTextures[0] = (diffuseTexture);

    Texture* specularTexture = new Texture(glm::vec3(255, 255, 255));
    sTextures[0] = (specularTexture);

    InitDescriptorSets();
}

Mesh::Mesh(std::string path) {
    InitUniformBuffers();


    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;

    for (size_t i = 0; i < scene->mNumMeshes; i++) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

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

    InitDescriptorSets();
}

Mesh::Mesh(std::string path, glm::vec3 color) {
    InitUniformBuffers();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        std::cout << "Error assimp: " << importer.GetErrorString() << std::endl;

    for (size_t i = 0; i < scene->mNumMeshes; i++) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

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

    InitDescriptorSets();
}

void Mesh::Draw(VkCommandBuffer commandBuffer, VkPipeline pipeline, VkPipelineLayout Layout, int imageIndex) {
    
    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        pipeline
    );
  
    
    for (size_t i = 0; i < material_ID.size(); i++)
    {
        VkBuffer buffers[] = { vertexArrays[i]->GetVertexBuffer() };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);


        VkDescriptorSet Sets[] = { uniformDescriptorSets[0], materialDescriptorSets[material_ID[i]][0] };
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Layout,
            0, 2, Sets, 0, nullptr);

         /*
         DataTypes::PushConstants constants;
         constants.diffuseMapId = Shapes[i].MatID;*/

        //vkCmdPushConstants(commandBuffer, renderer.graphicsPipelineForMesh.GetPipelineLayout(),
        //    VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(constants), &constants);

        
        vkCmdBindIndexBuffer(commandBuffer, vertexArrays[i]->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
        vkCmdDrawIndexed(commandBuffer, (uint32_t)vertexArrays[i]->GetIndices().size(), 1, 0, 0, 0);
    }
}

Mesh::~Mesh() {
    ClearDescriptorSets();
    ClearUniformBuffers();
    

    std::map<int, Texture*>::iterator it;
    for (it = dTextures.begin(); it != dTextures.end(); it++)
    {
        delete it->second;
    }

    for (it = sTextures.begin(); it != sTextures.end(); it++)
    {
        delete it->second;
    }

    for (size_t i = 0; i < vertexArrays.size(); i++)
    {
        delete vertexArrays[i];
    }
}
#endif