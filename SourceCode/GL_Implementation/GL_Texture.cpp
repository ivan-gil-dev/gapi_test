#include "../API/Texture.h"

#define STB_IMAGE_IMPLEMENTATION  
#include <stb_image.h>


#ifdef USE_GL
Texture::Texture(std::string path) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channelCount;
    unsigned char* img;
    
    img = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
    
    if (img) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (channelCount == 3)  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        if (channelCount == 1)  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_R8, GL_UNSIGNED_BYTE, img);
        if (channelCount == 4)  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(img);
    }
    else {
        img = new unsigned char[3];
        img[0] = 252;
        img[1] = 2;
        img[2] = 252;
        width = 1;
        height = 1;

        std::cout << "Не удалось загрузить изображение" << std::endl;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
        delete[]img;
    }
    }

Texture::Texture(glm::vec3 color) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    unsigned char* img;
    img = new unsigned char[3];
    img[0] = color.r;
    img[1] = color.g;
    img[2] = color.b;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[]img;
}

void Texture::BindTexture(unsigned int textureSlot) {
    if (texture) {
        glActiveTexture(textureSlot);
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);
}
#endif