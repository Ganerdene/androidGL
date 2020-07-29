//
// Created by Maco on 7/23/2020.
//

#ifndef MYFIRSTNATIVE_RESOURCEMANAGER_H
#define MYFIRSTNATIVE_RESOURCEMANAGER_H

#include <string>
#include <map>
#include "Shader.h"
#include "Texture.h"
#include <android/asset_manager_jni.h>
#include <GLES3/gl32.h>

class ResourceManager {
public:
    // resource storage
    static std::map<std::string, Shader>    Shaders;
    static std::map<std::string, Texture2D> Textures;

    static void set_asset_manager(AAssetManager* asset_manager);

    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader    LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, std::string name);
    // retrieves a stored sader
    static Shader    GetShader(std::string name);

    // loads (and generates) a texture from file
    static Texture2D LoadTexture(const char *file, bool alpha, std::string name);
    // retrieves a stored texture
    static Texture2D GetTexture(std::string name);
    // properly de-allocates all loaded resources
    static void      Clear();
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    // loads and generates a shader from file
    static Shader    LoadShaderFromFile(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile = nullptr);
    // loads a single texture from file
    static Texture2D LoadTextureFromFile(const char *file, bool alpha);

    static std::string Load_file(const GLchar *file_path);

    static AAssetManager *m_asset_manager;
};


#endif //MYFIRSTNATIVE_RESOURCEMANAGER_H
