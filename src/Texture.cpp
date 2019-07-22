#include <iostream>

#include "Texture.hpp"

Texture2D::Texture2D()
        : width_(0), height_(0), internal_format_(GL_RGB), image_format_(GL_RGB), wrap_s_(GL_REPEAT), wrap_t_(GL_REPEAT), filter_min_(GL_LINEAR_MIPMAP_LINEAR), filter_max_(GL_LINEAR) {
    glGenTextures(1, &this->id_);
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char * data) {
    this->width_ = width;
    this->height_ = height;
    // Create Texture
    glBindTexture(GL_TEXTURE_2D, this->id_);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internal_format_, width, height, 0, this->image_format_, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // Set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrap_s_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrap_t_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filter_min_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filter_max_);
    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const {
    glBindTexture(GL_TEXTURE_2D, this->id_);
}
