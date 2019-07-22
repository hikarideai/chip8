#include <sstream>
#include <fstream>
#include <iostream>

#include "ResourceManager.hpp"

#ifdef STB_IMAGE_SUPPORT
#include "stb_image.h"
#endif

std::map<std::string, Texture2D> ResourceManager::textures_;
std::map<std::string, Shader>    ResourceManager::shaders_;

Shader & ResourceManager::LoadShader(const GLchar * vFile, const GLchar * fFile, const GLchar *gFile, std::string name) {
    // Detect if vertex shader exists
    std::ifstream test(vFile);
    if (test.bad())
        std::cerr << "ERROR::ResourceManager: There's no vertex shader at " << vFile << std::endl;
    test.close();
    // Fragment shader
    test.open(fFile);
    if (test.bad())
        std::cerr << "ERROR::ResourceManager: There's no fragment shader at " << vFile << std::endl;
    test.close();
    // And geometry shader too!
    if (gFile) {
        test.open(gFile);
        if (test.bad())
            std::cerr << "ERROR::ResourceManager: There's no geometry shader at " << vFile << std::endl;
        test.close();
    }
    shaders_[name] = loadShaderFromFile(vFile, fFile, gFile);
    return shaders_[name];
}

Shader & ResourceManager::GetShader(std::string name) {
    if (shaders_.count(name) == 0)
        std::cerr << "WARNING::ResourceManager: Requested \"" << name << "\" shader haven't been loaded." << std::endl;
    return shaders_[name];
}

#ifdef STB_IMAGE_SUPPORT
Texture2D & ResourceManager::LoadTexture(const GLchar * file, GLboolean alpha, std::string name) {
    std::fstream test(file);
    if (test.bad())
        std::cerr << "ERROR::ResourceManager: There's no texture at " << file << std::endl;
    test.close();
    textures_[name] = loadTextureFromFile(file, alpha);
    return textures_[name];
}
#endif // STB_IMAGE_SUPPORT

Texture2D & ResourceManager::LoadTexture(Texture2D texture, const std::string & name) {
    textures_[name] = texture;
    return textures_[name];
}

Texture2D & ResourceManager::GetTexture(std::string name) {
    if (textures_.count(name) == 0)
        std::cerr << "WARNING::ResourceManager: Requested \"" << name << "\" texture haven't been loaded." << std::endl;
    return textures_[name];
}

void ResourceManager::Clear() {
    for (auto &i : textures_)
        glDeleteTextures(1, &i.second.id_);
    for (auto &i : shaders_)
        glDeleteProgram(i.second.id_);
}

Shader ResourceManager::loadShaderFromFile(const GLchar * vPath, const GLchar * fPath, const GLchar * gPath) {
    std::string vert_src, frag_src, gmt_src;
    // Retrieve source code
    try {
        // Open files
        std::ifstream vFile(vPath);
        std::ifstream fFile(fPath);
        std::stringstream vStream, fStream;
        // Read file buffer's contents into streams
        vStream << vFile.rdbuf();
        fStream << fFile.rdbuf();
        // Close file handlers
        vFile.close();
        fFile.close();
        // Convert string into stream
        vert_src = vStream.str();
        frag_src = fStream.str();
        // If geometry shader path is provided, load it also
        if (gPath) {
            std::ifstream gFile(gPath);
            std::stringstream gStream;
            gStream << gFile.rdbuf();
            gFile.close();
            gmt_src = gStream.str();
        }
    } catch (std::exception e) {
        std::cout << "ERROR::SHADER: Failed to read shader source files." << std::endl;
    }
    const GLchar * vCode = vert_src.c_str(),
                 * fCode = frag_src.c_str(),
                 * gCode = gmt_src.c_str();
    // Create shader object from the sources
    Shader shader;
    shader.Compile(vCode, fCode, gPath ? gCode : nullptr);
    return shader;
}

#ifdef STB_IMAGE_SUPPORT
Texture2D ResourceManager::loadTextureFromFile(const GLchar * file, GLboolean alpha) {
    // Create texture object
    Texture2D texture;
    if (alpha == GL_TRUE) {
        texture.internal_format_ = GL_RGBA;
        texture.image_format_ = GL_RGBA;
    }
    // Load image
    int width, height;
    unsigned char * pixels = stbi_load(file, &width, &height, NULL, 0);
    if (!pixels) {
        std::cerr << "ERROR::ResourseManager: Unable to load texture at " << file << std::endl;
    }
    // Generate the texture
    texture.Generate(width, height, pixels);
    // Free image data
    stbi_image_free(pixels);
    return texture;
}
#endif // STB_IMAGE_SUPPORT
