#pragma once

#include "../DataTypes.h"
#include <vector>
#include <string>

#include <iostream>
#ifdef USE_GL
#include <glad/glad.h>;
#endif

class Texture {

#ifdef USE_GL
    GLuint  texture;
#endif
    
public:
    Texture(std::string path);

    Texture(glm::vec3 color);

    void BindTexture(unsigned int textureSlot);

    ~Texture();

};