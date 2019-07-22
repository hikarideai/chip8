#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#define STB_IMAGE_SUPPORT

#include <map>
#include <string>

#include "Texture.hpp"
#include "Shader.hpp"

class ResourceManager {
public:
    //Resource storage
    static std::map<std::string, Texture2D> textures_;
    static std::map<std::string, Shader> shaders_;

    // Loads (and generates) shader program from vertex', geometry's and fragment's shaders source codes.
    // Geometry shader can be nullptr
    static Shader & LoadShader(const GLchar * vFile, const GLchar * fFile, const GLchar * gFile, std::string name);
    // Retrieves a stored shader
    static Shader & GetShader(std::string name);
    // Adds already existing texture to the cache
    static Texture2D & LoadTexture(Texture2D texture, const std::string & name);
    // Retrieves a stored texture
    static Texture2D & GetTexture(std::string name);
    // Properly deallocates all loaded resources
    static void Clear();

    #ifdef STB_IMAGE_SUPPORT
    // Loads (and generates) texture from file
    static Texture2D & LoadTexture(const GLchar * file, GLboolean alpha, std::string name);
    #endif

private:
    // Private constructor ensures that nobody can instantiate ResourceManager class.
    ResourceManager() = default;
    // Loads and generates shader from file
    static Shader loadShaderFromFile(const GLchar * vFile, const GLchar * fFile, const GLchar * gFile = nullptr);

    #ifdef STB_IMAGE_SUPPORT
    // Loads a single texture from file
    static Texture2D loadTextureFromFile(const GLchar * file, GLboolean alpha);
    #endif
};

#endif
