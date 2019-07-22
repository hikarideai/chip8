#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include "glad/glad.h"

class Texture2D {
public:
    // Holds the id of the texture, is required for all operations on the terxture.
    GLuint id_;
    // Texture image dimensions
    GLuint width_, height_;
    // Texture format
    GLuint internal_format_;    // Format of the texture object
    GLuint image_format_;       // Format of the loaded image
    // Texture configuration
    GLuint wrap_s_;             // Wrapping on the S axis
    GLuint wrap_t_;             // Wrapping on the T axis
    GLuint filter_min_;         // Filtering mode if texture pixels < screen pixels
    GLuint filter_max_;         // Filtering mode if texture pixels > screen pixels
    // Constructor (sets default texture modes)
    Texture2D();
    // Generates texture from image data
    void Generate(GLuint width, GLuint height, unsigned char * data);
    // Binds the texture as the current GL_TEXTURE_2D texture object
    void Bind() const;
};

#endif
